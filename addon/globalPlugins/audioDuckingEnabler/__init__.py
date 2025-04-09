import globalPluginHandler
import audioDucking
import config
import synthDriverHandler
from logHandler import log
import ctypes
import os


def _isAudioDuckingSupported():
	return True


class GlobalPlugin(globalPluginHandler.GlobalPlugin):
	def __init__(self):
		super(GlobalPlugin, self).__init__()
		self._hookDll = None
		self._original_isAudioDuckingSupported = None
		try:
			self._hookDll = ctypes.windll.LoadLibrary(os.path.join(os.path.dirname(__file__), "hookDll.dll"))
			self._original_isAudioDuckingSupported = audioDucking.isAudioDuckingSupported
			audioDucking.isAudioDuckingSupported = _isAudioDuckingSupported
			log.info("Audio ducking support patched successfully")
			# Reload the synthesizer
			audioDucking.setAudioDuckingMode(config.conf["audio"]["audioDuckingMode"])
			currentSynth = synthDriverHandler.getSynth()
			synthDriverHandler.setSynth(currentSynth.name)
		except Exception:
			log.warn("Audio ducking cannot be enabled", exc_info=True)

	def terminate(self):
		super(GlobalPlugin, self).terminate()
		if self._original_isAudioDuckingSupported:
			audioDucking.isAudioDuckingSupported = self._original_isAudioDuckingSupported
		if self._hookDll:
			handle = self._hookDll._handle
			del self._hookDll
			ctypes.windll.kernel32.FreeLibrary(handle)
