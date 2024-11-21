extends Node

onready var voice_audio = load("res://Scenes/Entities/Player/SpeechBubble/voice_audio.tscn")

var DecTalk

func add_script_node(script):
	var node = Node2D.new()
	node.set_script(script)
	add_child(node)
	return node

func _ready():
	# Thanks NotNite for the code from https://github.com/NotNite/WebfishingRichPresence/blob/13abe24509360eea518d1c207b8fed9d4e415ac2/project/mods/WebfishingRichPresence/main.gd
	# And thanks Awesomerly for sending this to me
	var lib = GDNativeLibrary.new()
	var cfg = ConfigFile.new()
	cfg.set_value("entry", "Windows.64", "%LIBRPCPATH%")
	lib.config_file = cfg

	var script = NativeScript.new()
	script.library = lib
	script.resource_name = "DecTalk"
	script.set_class_name("DecTalk")
	
	DecTalk = add_script_node(script)
	DecTalk.transform.origin = Vector3.ZERO

func speak(text: String):
	var new = voice_audio.instance()
	
	var generator = AudioStreamGenerator.new()
	generator.mix_rate = 44100
	generator.buffer_length = 10
	
	new.stream = generator;
	DecTalk.Speak(text, new.get_stream_playback())
	return new
