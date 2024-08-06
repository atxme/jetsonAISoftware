###
# import torch
#from torch2trt import TRTModule

#model_trt = TRTModule()
#model_trt.load_state_dict(torch.load('best_steering_model_xy_trt.pth')) # well trained road following model

#model_trt_collision = TRTModule()
#model_trt_collision.load_state_dict(torch.load('best_model_trt.pth')) # well trained collision avoidance model
###

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
        with open(engine_path, "rb") as f, trt.Runtime(TRT_LOGGER) as runtime:
            self.engine = runtime.deserialize_cuda_engine(f.read())
        self.context = self.engine.create_execution_context()

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

    def infer(self, img):
        try:
            # Check if image is loaded correctly
            if img is None or not isinstance(img, np.ndarray):
                raise ValueError("Invalid image input. Must be a valid numpy array.")
            
            # Preprocess the image using OpenCV and CUDA
            cuda_image = cv2.cuda_GpuMat()
            cuda_image.upload(img)

            # Convert image to RGB
            cuda_image = cv2.cuda.cvtColor(cuda_image, cv2.COLOR_BGR2RGB)

            # Resize the image to the expected input size (e.g., 224x224)
            cuda_image = cv2.cuda.resize(cuda_image, (224, 224))

            # Download the processed image from GPU to CPU
            preprocessed_image = cuda_image.download()

            # Normalize and transpose the image as required by the model
            preprocessed_image = preprocessed_image.astype(np.float32) / 255.0
            preprocessed_image = np.transpose(preprocessed_image, (2, 0, 1))
            preprocessed_image = np.expand_dims(preprocessed_image, axis=0)

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
