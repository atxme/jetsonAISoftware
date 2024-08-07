import gi
import asyncio
from gi.repository import Gst, GstWebRTC, GLib
import aiohttp
from videoFlux import VideoFlux

gi.require_version('Gst', '1.0')
gi.require_version('GstWebRTC', '1.0')

Gst.init(None)

class GstWebRTCClient:
    def __init__(self, server_url: str, videoFlux):
        self.server_url = server_url
        self.videoFlux = videoFlux
        self.pipeline = None
        self.webrtc = None
        self.loop = GLib.MainLoop()
        self.sdp_offer = None

    def create_pipeline(self):
        # Create GStreamer pipeline
        self.pipeline = Gst.parse_launch(
            'appsrc name=src ! videoconvert ! x264enc ! rtph264pay ! webrtcbin name=webrtc'
        )
        self.webrtc = self.pipeline.get_by_name('webrtc')
        src = self.pipeline.get_by_name('src')

        # Configure WebRTC
        self.webrtc.connect('on-negotiation-needed', self.on_negotiation_needed)
        self.webrtc.connect('on-ice-candidate', self.on_ice_candidate)

        # Set pipeline state to PLAYING
        self.pipeline.set_state(Gst.State.PLAYING)

        # Configure appsrc to push frames from VideoFlux
        self.configure_appsrc(src)

    def configure_appsrc(self, appsrc):
        appsrc.set_property('caps', Gst.Caps.from_string('video/x-raw,format=BGR'))
        appsrc.set_property('stream-type', 0)  # Set to STREAM
        appsrc.set_property('do-timestamp', True)

        # Create a new thread to push frames to appsrc
        def push_frames():
            while True:
                frame = self.videoFlux.queue.get()
                buffer = Gst.Buffer.new_wrapped(frame.tobytes(), len(frame.tobytes()), 0)
                appsrc.emit('push-buffer', buffer)
        
        from threading import Thread
        thread = Thread(target=push_frames)
        thread.daemon = True
        thread.start()

    def on_negotiation_needed(self, webrtcbin):
        # Create and set offer
        offer = webrtcbin.emit('create-offer')
        offer.connect('promise', self.on_offer_created)

    def on_offer_created(self, promise):
        offer = promise.get()
        self.sdp_offer = offer.sdp
        asyncio.run(self.send_offer_to_server(offer.sdp))

    async def send_offer_to_server(self, sdp):
        async with aiohttp.ClientSession() as session:
            async with session.post(f'{self.server_url}/offer', json={'sdp': sdp, 'type': 'offer'}) as response:
                answer = await response.json()
                # Set remote description
                self.webrtc.emit('set-remote-description', GstWebRTC.WebRTCSessionDescription.new('answer', answer['sdp']))

    def on_ice_candidate(self, webrtcbin, mlineindex, candidate):
        print(f"ICE candidate: {candidate}")
        # Handle ICE candidates here if needed

    def run(self):
        self.create_pipeline()
        self.loop.run()