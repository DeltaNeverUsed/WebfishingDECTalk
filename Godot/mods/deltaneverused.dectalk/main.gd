extends Node

onready var voice_audio = load("res://Scenes/Entities/Player/SpeechBubble/voice_audio.tscn")

var DecTalk

func move_dectalk_files(target_folder_name: String) -> void:
	var exe_dir = OS.get_executable_path().get_base_dir()
	
	var dir = Directory.new()
	# Search for the folder in the executable directory
	var found_path = find_folder_recursive(exe_dir, target_folder_name)
	
	if found_path.length() != 0:
		var args = ['"' + found_path + "\\*\"", '"' + exe_dir + "\\\"", "/E", "/H", "/C", "/I"]
		print("xcopy " + found_path + "\\* " + exe_dir + "\\", "/E", "/H", "/C", "/I")
		OS.execute("xcopy", args, true)
	else:
		print("Folder not found.")

func find_folder_recursive(base_path: String, folder_name: String) -> String:
	var dir = Directory.new()
	if dir.open(base_path) != OK:
		print("Failed to open directory:", base_path)
		return ""
	
	dir.list_dir_begin(true, true)
	while true:
		var item = dir.get_next()
		if item.length() == 0:
			break  # No more items
		
		var item_path = base_path.plus_file(item)
		
		if dir.current_is_dir():
			if item == folder_name:
				return item_path  # Found the folder
		
			# Recursively search this subdirectory
			var result = find_folder_recursive(item_path, folder_name)
			if result.length() != 0:  # If found in a subdirectory, return immediately
				dir.list_dir_end()
				return result
	dir.list_dir_end()
	return ""

func add_script_node(path: String):
	var node = Node2D.new()
	node.set_script(load(path))
	add_child(node)
	return node

func _ready():
	var dir = Directory.new()
	if dir.open(OS.get_executable_path().get_base_dir() + "dic") != OK:
		move_dectalk_files("DECTalkStuffHere")
	
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
