script moongate
{
	//Set the initial frame (when it first appears in the ground):
	frame 0;

	//The moongate has 12 frames (0-based); go through them all:
	repeat 10 next frame cycle;;
	//Note that there are TWO ";" in the above statement; one is from the
	//repeat, while the other is from the command. Also, it is important
	//to note that repeat will execute the commands once and then *repeat*
	//them the specified number of times.

	//The moongate has fully risen, so now it is time to animate it;
	//frames 4 to 11 can be cycled continuously.

	//We will display the full cycle 5 additional times:
	repeat 5
	{
		//Return to the starting frame of the cycle:
		frame 4;

		//Go through each frame of the animation. It is done this way
		repeat 5	next frame cycle;;
	};

	//Now time to make the moongate disappear; return to the first
	//standing frame:
	frame 4;

	//Decrement the frame until frame 0, which is when the moongate is
	//about to vanish:
	repeat 3 previous frame cycle;;

	//Delete the moongate, making it vanish:
	remove;
}