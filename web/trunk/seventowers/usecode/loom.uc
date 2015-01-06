script item			//item is the loom
{
	//Set the initial frame:
	frame 0;

	//Repeat the following commands 32 times each in sequence.
	//The loom has 16 frames; starting from frame zero, every
	//16 repeats returns the loom to frame zero; so the cycle
	//ends in frame zero.
	repeat 32
	{
		next frame cycle;
		continue;
		sfx 6;
	};

	//After all the above is done, call again the Loom function
	//to create the cloth:
	call Loom;		//Loom function # is 261 = 0x0105
}

//Determine which way the avatar must face:
direction = getDirectionDirectionToObject(AVATAR, item);

script AVATAR
{
	//Turn the avatar to the appropriate direction:
	face direction;

	//The "looming" animation of the avatar, repeated
	//nine times.
	repeat 9
	{
		continue;
		actor frame 6;
		actor frame 0;
		wait 1;
	};
}