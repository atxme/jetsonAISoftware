from aiortc import RTCPeerConnection, RTCSessionDescription
from aiortc.contrib.media import MediaPlayer, MediaBlackhole
from aiortc import web 
import json 


class RtcSession :

    def __init__(self) -> None:
        self.pc = RTCPeerConnection()
        self.player = MediaPlayer('/dev/video0')
        self.blackhole = MediaBlackhole()
        self.pc.addTrack(self.player.audio)
        self.pc.addTrack(self.player.video)