package com.exult.android;
import java.io.RandomAccessFile;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.io.BufferedOutputStream;
import java.io.InputStream;
import java.io.FileInputStream;
import java.io.PushbackInputStream;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.IOException;
import java.util.TreeMap;
import java.util.Random;
import java.util.Vector;
import java.util.regex.Pattern;

public class EUtil {
	private static TreeMap<String,String> pathMap;
	private static byte buf2[] = new byte[2];
	private static byte buf4[] = new byte[4];
	private static Random random = new Random();
	public static final int Read2(byte buf[], int ind) {
		return ((int)buf[ind]&0xff) | ((int)buf[ind+1]&0xff)<<8;
	}
	public static final int Read4(byte buf[], int ind) {
		return (((int)buf[ind]&0xff) | (((int)buf[ind+1]&0xff)<<8) | 
				(((int)buf[ind+2]&0xff)<<16) | (((int)buf[ind+3]&0xff)<<24));
	}
	public static final int Write4(byte buf[], int ind, int v) {
		buf[ind] = (byte)(v&0xff);
		buf[ind+1] = (byte)((v>>8)&0xff);
		buf[ind+2] = (byte)((v>>16)&0xff);
		buf[ind+3] = (byte)((v>>24)&0xff);
		return ind + 2;
	}
	public static final int Write2(byte buf[], int ind, short v) {
		buf[ind] = (byte)(v&0xff);
		buf[ind+1] = (byte)((v>>8)&0xff);
		return ind + 2;
	}
	public static final int Write2(byte buf[], int ind, int v) {
		buf[ind] = (byte)(v&0xff);
		buf[ind+1] = (byte)((v>>8)&0xff);
		return ind + 2;
	}
	public static final void Memcpy(byte dest[], int out, byte src[], int in, int cnt) {
		while (cnt > 0) {
			dest[out] = src[in];
			++out; ++in;
			--cnt;
		}
	}
	//++++Hoping these randomaccessfile methods can go away.
	public static final int Read4(RandomAccessFile in) {
		try {
			in.read(buf4);
			return (((int)buf4[0]&0xff) | (((int)buf4[1]&0xff)<<8) | 
					(((int)buf4[2]&0xff)<<16) | (((int)buf4[3]&0xff)<<24));
		} catch (IOException e) {
			return -1;
		}
	}
	public static final int Read2(RandomAccessFile in) {
		try {
			in.read(buf2);
			return (((int)buf2[0]&0xff) | (((int)buf2[1]&0xff)<<8));
		} catch (IOException e) {
			return -1;
		}
	}
	public static final int Read4(DataSource in) {
		try {
			in.read(buf4);
			return (((int)buf4[0]&0xff) | (((int)buf4[1]&0xff)<<8) | 
					(((int)buf4[2]&0xff)<<16) | (((int)buf4[3]&0xff)<<24));
		} catch (IOException e) {
			return -1;
		}
	}
	public static final int Read2(DataSource in) {
		try {
			in.read(buf2);
			return (((int)buf2[0]&0xff) | (((int)buf2[1]&0xff)<<8));
		} catch (IOException e) {
			return -1;
		}
	}
	public static final int Read4(InputStream in) {
		try {
			in.read(buf4);
			return (((int)buf4[0]&0xff) | (((int)buf4[1]&0xff)<<8) | 
					(((int)buf4[2]&0xff)<<16) | (((int)buf4[3]&0xff)<<24));
		} catch (IOException e) {
			return -1;
		}
	}
	public static final int Read2(InputStream in) {
		try {
			in.read(buf2);
			return (((int)buf2[0]&0xff) | (((int)buf2[1]&0xff)<<8));
		} catch (IOException e) {
			return -1;
		}
	}
	public static final int Read1(InputStream in) {
		try {
			return ((int)in.read()&0xff);
		} catch (IOException e) {
			return -1;
		}
	}
	public static final void Write2(OutputStream out, int v) throws IOException {
		out.write(v&0xff);
		out.write((v>>8)&0xff);
	}
	public static final void Write4(OutputStream out, int v) throws IOException {
		out.write(v&0xff);
		out.write((v>>8)&0xff);
		out.write((v>>16)&0xff);
		out.write((v>>24)&0xff);
	}
	// Peek, but put back.
	public static final int peek(PushbackInputStream in) {
		try {
			int p = in.read();
			in.unread(p);
			return p;
		} catch (IOException e) {
			return -1;
		}
	}
	// Read integer from a text file, skipping past the '/' after the number.
	public static final int ReadInt(PushbackInputStream in) {
		return ReadInt(in, 0);
	}
	public static final int ReadInt(PushbackInputStream in, int def) {
		int b;
		boolean neg = false;
		int i = 0, digits = 0;
		try {
			while (true) {
				b = (int)in.read()&0xff;
				if (b == '-' && digits == 0)
					neg = !neg;
				else if (Character.isDigit(b)) {
					i = 10*i + (b - (int)'0');
					++digits;
				} else if (digits != 0 || !Character.isSpace((char)b)) {
					if (b != -1)
						in.unread(b);
					break;
				}
			}
		} catch (IOException e) { }
		try {
			while ((b = in.read()) != '/' && b != -1);
		} catch (IOException e) { }
		if (neg)
			i = -i;
		return digits > 0 ? i : def;
	}
	
	public static final String ReadStr(InputStream in) {
		StringBuffer buf = new StringBuffer(50);
		int c;
		try {
			while ((c = in.read()) != '/' && c != -1)
				buf.append((char) c);
		} catch (IOException e) { }
		return buf.toString();
	}
	private static String baseToUppercase(String str, int count) {
		if (count <= 0) return str;
		int todo = count, i;
						// Go backwards.
		for (i = str.length() - 1; i >= 0; --i) {
			int c = str.charAt(i);
			if (c == '/') {
				todo--;
				if (todo <= 0)
					break;
			}
		}
		if (todo > 0)
			return null;	// Didn't reach 'count' parts.
		String res = str.substring(0, i) + str.substring(i, str.length()).toUpperCase();
		return res;
	}

	public static final String U7exists(String nm) {
		String name = getSystemPath(nm);
		int uppercasecount = 0;
		do {
			if (new File(name).exists())
				return name; // found it!
		} while ((name = baseToUppercase(name, ++uppercasecount)) != null);
		return null;
	}
	//+++++Hoping this one can go away.
	public static final boolean isFlex(RandomAccessFile in) {
		int magic = 0;
		try {
			long pos = in.getFilePointer();
			long len = in.length();	// Check length.
			if (len >= 0x80L) {		// Has to be at least this long.
				in.seek(0x50);
				magic = EUtil.Read4(in);
			}
			in.seek(pos);
		} catch (IOException e) {}
		return (magic==0xffff1a00); 
	}
	public static final boolean isFlex(DataSource in) {
		int magic = 0;
		try {
			long pos = in.getFilePointer();
			long len = in.length();	// Check length.
			if (len >= 0x80L) {		// Has to be at least this long.
				in.seek(0x50);
				magic = EUtil.Read4(in);
			}
			in.seek(pos);
		} catch (IOException e) {}
		return (magic==0xffff1a00); 
	}
	public static final boolean isFlex(String nm) {
		RandomAccessFile file = null;
		String fname = U7exists(nm);
		if (fname != null) {
			try {
				file = new RandomAccessFile(fname, "r");
				return isFlex(file);
			} catch (IOException e) {
				return false;
			}
		}
		return false;
	}
	public static boolean isSystemPathDefined(String path) {
		return pathMap != null && pathMap.containsKey(path);
	}
	public static String getSystemPath(String path) {
		String newPath;
		int pos, pos2;
		pos = path.indexOf('>');
		pos2 = path.indexOf('<');
		// If there is no separator, return the path as is
		if (pos == -1 || pos2 != 0) {
			newPath = path;
		} else {
			pos += 1;
			// See if we can translate this prefix
			String syspath = path.substring(0, pos);
			if (isSystemPathDefined(syspath)) {
				String newPrefix = pathMap.get(syspath);
				newPath = newPrefix + path.substring(pos);
			} else {
				newPath = path;
			}
		}
		return newPath;
	}
	public static void addSystemPath(String key, String value) {
		if (pathMap == null)
			pathMap = new TreeMap<String,String>();
		pathMap.put(key, value);
	}
	public static void initSystemPaths() {
		String base = "/sdcard/Games/exult";
		String bgbase = base + "/blackgate";	// FOR NOW.
		addSystemPath("<DATA>", base);
		addSystemPath("<MUSIC>", base + "/MUSIC");
		addSystemPath("<SPEECH>", base + "/SPEECH");		
		addSystemPath("<VIDEO>", base + "/VIDEO");
		addSystemPath("<PATCH>", bgbase + "/PATCH");
		addSystemPath("<STATIC>", bgbase + "/STATIC");
		addSystemPath("<GAMEDAT>", bgbase + "/GAMEDAT");
		addSystemPath("<SAVEGAME>", bgbase);
	}
	public static RandomAccessFile U7open(String nm, boolean hardfail)
												throws IOException {
		String fname = U7exists(nm);
		if (fname != null) try {
			return new RandomAccessFile(fname, "r");
		} catch (IOException e) { 
			if (hardfail)
				throw e;
		}
		return null;
	}
	// First try nm1, then nm2.  Returns null if neither found.
	public static RandomAccessFile U7open2(String nm1, String nm2) {
		String nm = U7exists(nm1);
		if (nm != null) try {
			return new RandomAccessFile(nm, "r");
		} catch (IOException e) { }
		nm = U7exists(nm2);
		if (nm != null) try {
			return new RandomAccessFile(nm, "r");
		} catch (IOException e) { }
		return null;
	}
	public static InputStream U7openStream(String nm)
								throws IOException {
		String fname = getSystemPath(nm);
		return new BufferedInputStream(new FileInputStream(fname), 0x8000);
	}
	// First try nm1, then nm2.  Returns null if neither found.
	public static InputStream U7openStream2(String nm1, String nm2) {
		String nm = U7exists(nm1);
		if (nm != null) try {
			return new BufferedInputStream(new FileInputStream(nm), 0x8000);
		} catch (IOException e) { }
		nm = U7exists(nm2);
		if (nm != null) try {
			return new BufferedInputStream(new FileInputStream(nm), 0x8000);
		} catch (IOException e) { }
		return null;
	}
	public static void U7remove(String nm) {
		String fname = U7exists(nm);
		if (fname != null) {
			File f = new File(fname);
			f.delete();
		}
	}
	public static boolean U7mkdir(String nm) {
		if (U7exists(nm) != null)
			return true;
		String fname = getSystemPath(nm);
		File f = new File(fname);
		return f.mkdir();
	}
	public static String baseName(String fname) {
		int split = fname.lastIndexOf('/');
		return split == -1 ? fname : fname.substring(split + 1);
	}
	//	Find files matching mask which is a REGEX pattern.
	public static void U7ListFiles(String mask, Vector<String> filelist) {
		mask = getSystemPath(mask);
		char sep = '/';
		int split = mask.lastIndexOf(sep);
		String dir, nameMask;
		if (split == -1) {
			dir = "."; nameMask = mask;
		} else {
			dir = mask.substring(0, split); nameMask = mask.substring(split + 1);
		}
		File folder = new File(dir);
	    File[] listOfFiles = folder.listFiles();
	    Pattern pattern = Pattern.compile(nameMask);
	    for (int i = 0; i < listOfFiles.length; i++) {
	    	if (listOfFiles[i].isFile()) {
	    		String fname = listOfFiles[i].getName();
	    		if (pattern.matcher(fname).matches())
	    			filelist.add(dir+sep+fname);
	    	}
	    }
	}
	public static OutputStream U7create(String nm) throws IOException {
		String fname = getSystemPath(nm);
		return new BufferedOutputStream(new FileOutputStream(fname), 0x8000);
	}
	/*
	 *	Return the direction for a given slope (0-7).
	 *	NOTE:  Assumes cartesian coords, NOT screen coords. (which have y
	 *		growing downwards).
	 */
	public static final int getDirection(int deltay,int deltax) {
		if (deltax == 0)
			return deltay > 0 ? EConst.north : EConst.south;
		int dydx = (1024*deltay)/deltax;// Figure 1024*tan.
		if (dydx >= 0)
			if (deltax >= 0)	// Top-right quadrant?
				return dydx <= 424 ? EConst.east : dydx <= 2472 ? EConst.northeast
									: EConst.north;
			else			// Lower-left.
				return dydx <= 424 ? EConst.west : dydx <= 2472 ? EConst.southwest
									: EConst.south;
		else
			if (deltax >= 0)	// Lower-right.
				return dydx >= -424 ? EConst.east : dydx >= -2472 ? EConst.southeast
									: EConst.south;
			else			// Top-left?
				return dydx >= -424 ? EConst.west : dydx >= -2472 ? EConst.northwest
									: EConst.north;
	}
	/*
	 *	Return the direction for a given slope (0-7), rounded to NSEW.
	 *	NOTE:  Assumes cartesian coords, NOT screen coords. (which have y
	 *		growing downwards).
	 */
	public static final int getDirection4(int deltay, int deltax) {
		if (deltax >= 0)	// Right side?
			return (deltay > deltax ? EConst.north : deltay < -deltax ? EConst.south
									: EConst.east);
		else				// Left side.
			return (deltay > -deltax ? EConst.north : deltay < deltax ? EConst.south
									: EConst.west);
	}
	/*
	 *	Return the direction for a given slope (0-15).
	 *	NOTE:  Assumes cartesian coords, NOT screen coords. (which have y
	 *		growing downwards).
	 */
	public static final int getDirection16(int deltay, int deltax) {
		if (deltax == 0)
			return deltay > 0 ? 0 : 8;
		int dydx = (1024*deltay)/deltax;// Figure 1024*tan.
		int adydx = dydx < 0 ? -dydx : dydx;
		int angle = 0;
		if (adydx < 1533) {		// 1024*tan(5*11.25)
			if (adydx < 204)	// 1024*tan(11.25).
				angle = 4;
			else if (adydx < 684)	// 1024*tan(3*11.25).
				angle = 3;
			else
				angle = 2;
		} else {
			if (adydx < 5148)	// 1024*tan(7*11.25).
				angle = 1;
			else
				angle = 0;
		}
		if (deltay < 0)			// Check quadrants.
			if (deltax > 0)
				angle = 8 - angle;
			else
				angle += 8;
		else if (deltax < 0)
			angle = 16 - angle;
		return angle % 16;
	}

	public static final int rand() {
		return Math.abs(random.nextInt());
	}
	public static final int log2(int n) {
		int result = 0;
		for (n = n>>1; n != 0; n = n>>1)
			result++;
		return result;
	}
	private static final int TWO(int c)
		{ return (byte)(1<<c); }
	private static final int MASK(int c) {
		 return ((-1)) / (TWO(TWO(c)) + 1);
	}
	private static final int COUNT(int x, int c) {
		return ((x) & MASK(c)) + (((x) >> (TWO(c))) & MASK(c));
	}
	public static final int bitcount(byte v) {
			// Only works for 8-bit numbers.
		int n =  COUNT(v, 0) & 0xff;
		n = COUNT(n, 1) & 0xff;
		n = COUNT(n, 2) & 0xff;
		return n;
	}
}

