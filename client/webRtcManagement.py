import asyncio
import json
import cv2
from aiortc import RTCPeerConnection, RTCSessionDescription, VideoStreamTrack
from aiortc.contrib.media import MediaPlayer
from aiortc.mediastreams import VideoFrame
import aiohttp
from threading import Thread
from videoFlux import VideoFlux

class VideoFrameTrack(VideoStreamTrack):
    def __init__(self, videoFlux : VideoFlux):
        super().__init__()
        self.videoFlux = videoFlux

    async def recv(self):
        frame = self.videoFlux.queue.get()
        pts, time_base = await self.next_timestamp()
        return VideoFrame.from_ndarray(frame, format="bgr24").rebuild(pts=pts, time_base=time_base)

class WebRTCClient:
    def __init__(self, server_url: str, videoFlux):
        self.server_url = server_url
        self.pc = RTCPeerConnection()
        self.video_flux = videoFlux
        self.video_track = VideoFrameTrack(videoFlux)

    async def login_to_host(self):
        async with aiohttp.ClientSession() as session:
            offer = await self.pc.createOffer()
            await self.pc.setLocalDescription(offer)

            async with session.post(f"{self.server_url}/offer", json={
                "sdp": self.pc.localDescription.sdp,
                "type": self.pc.localDescription.type,
            }) as response:
                answer = await response.json()

            await self.pc.setRemoteDescription(RTCSessionDescription(
                sdp=answer["sdp"], type=answer["type"]
            ))

            # Keep the connection open
            while True:
                await asyncio.sleep(1)

    def start(self):
        # Add the video track to the peer connection
        self.pc.addTrack(self.video_track)
        asyncio.run(self.login_to_host())