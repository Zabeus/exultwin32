#! /bin/bash

icotool -x -o ./thumbs *.ico
for num in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27; do
	fnd="\_$num\_"
	for nm in $(find ./ -name "*$fnd*"); do
		dest=${nm/$fnd/-}
		mv $nm $dest
	done
done

