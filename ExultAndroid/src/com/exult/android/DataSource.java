package com.exult.android;

import java.io.IOException;
import java.io.RandomAccessFile;

/*
 * This class acts like a RandomAccessFile but can represent a byte array.
 */
public abstract class DataSource {
	public abstract int read() throws IOException ;
	public abstract int read(byte buf[]) throws IOException ;
	public abstract void seek(long pos) throws IOException ;
	public abstract long length() throws IOException;
	public abstract long getFilePointer() throws IOException;
	public void close() throws IOException { }
	// Create from file.
	public static DataSource create(String fname) {
		try {
			RandomAccessFile file = EUtil.U7open(fname, true);
			return file == null ? null : new DataSource.File(file);
		} catch (IOException e) {
			return null;
		}
	}
	//	Create from resource within flex file, or whole file if rsc == -1.
	public static DataSource create(String fname, int rsc) {
		if (rsc == -1)
			return create(fname);
		
		byte buf[] = GameSingletons.fman.retrieve(fname, rsc);
		if (buf != null && buf.length != 0)
			return new DataSource.Buffer(buf);
		else
			return null;
	}
	public static class File extends DataSource {
		private RandomAccessFile file;
		
		public File(RandomAccessFile f) {
			file = f;
		}
		public int read() throws IOException {
			return file.read();
		}
		public int read(byte buf[]) throws IOException {
			return file.read(buf);
		}
		public void seek(long pos) throws IOException {
			file.seek(pos);
		}
		public long length() throws IOException {
			return file.length();
		}
		public long getFilePointer() throws IOException {
			return file.getFilePointer();
		}
		public void close() throws IOException {
			file.close();
		}
	}
	public static class Buffer extends DataSource {
		private byte data[];
		private int pos;
		
		public Buffer(byte b[]) {
			data = b;
			pos = 0;
		}
		public int read() throws IOException {
			if (pos >= data.length)
				return -1;
			return data[pos++];
		}
		public int read(byte buf[]) throws IOException {
			int max = data.length - pos;
			int cnt = buf.length < max ? buf.length : max;
			System.arraycopy(data, pos, buf, 0, cnt);
			pos += cnt;
			return cnt;
		}
		public void seek(long pos) throws IOException {
			this.pos = (int)pos;
		}
		public long length() throws IOException {
			return data.length;
		}
		public long getFilePointer() throws IOException {
			return pos;
		}
	}
}
