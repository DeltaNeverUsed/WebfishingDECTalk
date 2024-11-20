extends Node

onready var voice_audio = load("res://Scenes/Entities/Player/SpeechBubble/voice_audio.tscn")

var DecTalk

func add_script_node(path: String):
	var node = Node2D.new()
	node.set_script(load(path))
	add_child(node)
	return node

func _ready():
	DecTalk = add_script_node("res://mods/deltaneverused.dectalk/DecTalk.gdns")
	DecTalk.transform.origin = Vector3.ZERO

func speak(text: String):
	var new = voice_audio.instance()
	
	var generator = AudioStreamGenerator.new()
	generator.mix_rate = 11025
	generator.buffer_length = 10
	
	new.stream = generator;
	DecTalk.Speak(text, new.get_stream_playback())
	return new
