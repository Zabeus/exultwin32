package com.exult.android;

public class ShapeID extends GameSingletons {
	private short shapeNum;			// Shape #.
	private byte frameNum;			// Frame # within shape.
	private byte hasTrans;
	private ShapeFiles shapeFile;
	private ShapeFrame shape;
	// Shape_info *info;

	private ShapeFrame cacheShape() {
		if (frameNum == -1) return null;

		if (hasTrans != 2) hasTrans = 0;
		if (shapeFile == ShapeFiles.SHAPES_VGA) {
			shape = shapeFile.getFile().getShape(shapeNum, frameNum);
		/*	if (hasTrans != 2) 
				hasTrans = 
				    sman->shapes.get_info(shapenum).has_translucency();
		 */
		} else if (shapeFile != null) {
			shape = shapeFile.getFile().getShape(shapeNum, frameNum);
			if (shapeFile == ShapeFiles.SPRITES_VGA)
				hasTrans = 1;
		} else {
			// std::cerr << "Error! Wrong ShapeFile!" << std::endl;
			return null;
		} 
		return shape;
	}
	public final void set(int shnum, int frnum, ShapeFiles shfile) {
		shapeNum = (short)shnum; frameNum = (byte)frnum;
		hasTrans = 0;
		shape = null;
		shapeFile = shfile;	
	}
	public ShapeID(int shnum, int frnum, ShapeFiles shfile) {
		set(shnum, frnum, shfile);
	}
	public ShapeID(int shnum, int frnum) {
		set(shnum, frnum, ShapeFiles.SHAPES_VGA);
	}
	public ShapeID() {
		set(0,0, null);
	}
	public final boolean isInvalid()
		{ return shapeNum == -1; }

	public final int getShapeNum()
		{ return shapeNum; }
	public final int getFrameNum()
		{ return frameNum; }
	public final ShapeFiles getShapeFile()
		{ return shapeFile; }
	public final ShapeFrame getShape()
		{ return (shape!=null)?shape:cacheShape(); }
	public final ShapeInfo getInfo() {
		return ShapesVgaFile.getInfo(shapeNum);
	}
	public static final ShapeInfo getInfo(int shnum) {
		return ShapesVgaFile.getInfo(shnum);
	}
	public final void set_translucent(int trans)
		{ hasTrans = (byte)trans; }
	public final boolean isTranslucent()
		{ if (shape==null) cacheShape(); return hasTrans!=0; }
					// Set to given shape.
	public final void setShape(int shnum, int frnum) {
		shapeNum = (short)shnum;
		frameNum = (byte)frnum;
		shape = null;
		//info = null;
	}
	public final void setShape(int shnum)	// Set shape, but keep old frame #.
		{ shapeNum = (short)shnum; shape = null; /* info = 0; */ }
	public final void setFrame(int frnum)	// Set to new frame.
		{ frameNum = (byte)frnum; shape = null; }
	public final void setFile(ShapeFiles shfile)	// Set to new flex
		{ shapeFile = shfile; shape = null; }
	public final int get_num_frames() {
		if (shapeFile != null)
			return shapeFile.getFile().getNumFrames(shapeNum);
		else
			return 0;
	}
	public void paintShape(int xoff, int yoff) {
		ShapeFrame s = getShape();
		if (s != null)
			s.paint(gwin.getWin(), xoff, yoff);// ++++FOR NOW Orig. used Shape_manager.
	}
}