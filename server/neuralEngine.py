import cv2
import numpy as np
import tensorrt as trt
import pycuda.driver as cuda
import pycuda.autoinit

class NeuralEngine:

    def __init__(self) -> None:
        self.engine = None
        self.context = None
        self.inputs = []
        self.outputs = []
        self.bindings = []
        self.stream = None

    def load_engine(self, engine_path):
        TRT_LOGGER = trt.Logger(trt.Logger.INFO)
        try :
            with open(engine_path, "rb") as f, trt.Runtime(TRT_LOGGER) as runtime:
                self.engine = runtime.deserialize_cuda_engine(f.read())
                self.context = self.engine.create_execution_context()
        except Exception as e:
            print(f"An error occurred while loading the engine: {e}")
            return 

        for binding in self.engine:
            size = trt.volume(self.engine.get_binding_shape(binding)) * self.engine.max_batch_size
            dtype = trt.nptype(self.engine.get_binding_dtype(binding))
            host_mem = cuda.pagelocked_empty(size, dtype)
            device_mem = cuda.mem_alloc(host_mem.nbytes)
            self.bindings.append(int(device_mem))
            if self.engine.binding_is_input(binding):
                self.inputs.append({'host': host_mem, 'device': device_mem})
            else:
                self.outputs.append({'host': host_mem, 'device': device_mem})
        
        self.stream = cuda.Stream()

    def preprocess_image(self, img):
        # Convert image to RGB
        img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

        # Resize the image to the expected input size (e.g., 224x224)
        img_resized = cv2.resize(img_rgb, (224, 224))

        # Normalize and transpose the image as required by the model
        img_normalized = img_resized.astype(np.float32) / 255.0
        img_transposed = np.transpose(img_normalized, (2, 0, 1))
        img_expanded = np.expand_dims(img_transposed, axis=0)

        return img_expanded

    def infer(self, img):
        try:
            # Check if image is loaded correctly
            if img is None or not isinstance(img, np.ndarray):
                raise ValueError("Invalid image input. Must be a valid numpy array.")
            
            # Preprocess the image on CPU
            preprocessed_image = self.preprocess_image(img)

            # Ensure input shape matches
            if self.inputs:
                input_shape = self.engine.get_binding_shape(0)
                if preprocessed_image.shape != input_shape:
                    raise ValueError(f"Preprocessed image shape {preprocessed_image.shape} does not match model input shape {input_shape}.")

            # Copy data to the device input buffer
            np.copyto(self.inputs[0]['host'], preprocessed_image.ravel())
            cuda.memcpy_htod_async(self.inputs[0]['device'], self.inputs[0]['host'], self.stream)

            # Execute inference
            self.context.execute_async_v2(bindings=self.bindings, stream_handle=self.stream.handle)

            # Copy data from the device output buffer to the host
            cuda.memcpy_dtoh_async(self.outputs[0]['host'], self.outputs[0]['device'], self.stream)

            # Synchronize the stream to ensure all operations are complete
            self.stream.synchronize()

            return self.outputs[0]['host']
        except Exception as e:
            print(f"An error occurred during inference: {e}")
            return None
