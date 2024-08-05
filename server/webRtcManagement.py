import asyncio
import json
import aiohttp
import cv2
from aiortc import RTCPeerConnection, RTCSessionDescription
from aiortc.contrib.media import MediaBlackhole
from neuralEngine import NeuralEngine
from queue import Queue


class RtcSession:
    def __init__(self, server_url: str, neural_engine: NeuralEngine) -> None:
        self.server_url = server_url
        self.neural_engine = neural_engine
        self.pc = RTCPeerConnection()
        self.media_blackhole = MediaBlackhole()
        self.image_queue = Queue()  # Queue to store received images

        @self.pc.on("track")
        def on_track(track):
            print(f"Track {track.kind} received")

            if track.kind == "video":
                @track.on("frame")
                def on_frame(frame):
                    img = frame.to_ndarray(format="bgr24")
                    self.image_queue.put(img)  # Put the image in the queue

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
        asyncio.run(self.login_to_host())