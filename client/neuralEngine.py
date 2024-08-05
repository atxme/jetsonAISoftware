###
# import torch
from torch2trt import TRTModule

model_trt = TRTModule()
model_trt.load_state_dict(torch.load('best_steering_model_xy_trt.pth')) # well trained road following model

model_trt_collision = TRTModule()
model_trt_collision.load_state_dict(torch.load('best_model_trt.pth')) # well trained collision avoidance model
###