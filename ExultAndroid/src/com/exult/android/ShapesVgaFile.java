package com.exult.android;
import java.util.TreeMap;

/*
 * Represents THE 'shapes.vga' file.
 */
public class ShapesVgaFile extends VgaFile {
	private boolean infoRead;
	private static ShapeInfo info[];
	public ShapesVgaFile(
		String nm,				// Path to file.
		String nm2				// Patch file, or null.
		) {
		super(nm, nm2);
		info = new ShapeInfo[shapes.length];
		ShapeInfo.read(shapes.length, info);
	}
	public static ShapeInfo getInfo(int shapenum) {
		ShapeInfo s = info[shapenum];
		return s;
	}
}