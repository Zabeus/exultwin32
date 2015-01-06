UI_execute_usecode_array
	(
	item,
	[frame(0x0000),
	 next_frame,
	 cont,
	 sfx(0x0006),
	 repeat(-4, 0x0020),
	 usecode(0x0105)]
	);
direction = getDirectionDirectionToObject(AVATAR, item);
UI_execute_usecode_array
	(
	AVATAR,
	[face_dir(direction),
	 cont,
	 npcframe_6,
	 npcframe_0,
	 delay_ticks(0x0001),
	 repeat(-5, 0x0009)]
	);