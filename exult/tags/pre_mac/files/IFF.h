/*
Copyright (C) 2000  Dancer A.L Vesperman

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef __IFF_h_
#define __IFF_h_

#if !AUTOCONFIGURED
#include "../autoconfig.h"
#endif

#include <vector>
#include <string>
#include "common_types.h"
#include "U7file.h"


class   IFF    :       public virtual U7file
        {
public:
	struct  IFFhdr
		{
		char    form_magic[4];
		uint32  size;
		char    data_type[4];
		IFFhdr() {  }
		IFFhdr(const IFFhdr &i) : size(i.size)
			{
			memcpy(form_magic,i.form_magic,sizeof(form_magic));
			memcpy(data_type,i.data_type,sizeof(data_type));
			}
		IFFhdr &operator=(const IFFhdr &i)
			{
			size=i.size;
			memcpy(form_magic,i.form_magic,sizeof(form_magic));
			memcpy(data_type,i.data_type,sizeof(data_type));
			return *this;
			}
		};
	struct  IFFobject
		{
		char    type[4];
		uint32  size;
		char    even;
		};
	struct  u7IFFobj
		{
		char    name[8];
		// char    data[]; // Variable
		};
        struct Reference
                {
                uint32 offset;
                uint32 size;
                Reference() : offset(0),size(0) {};
                };
protected:
	IFFhdr	header;
        vector<Reference> object_list;
public:
        IFF(const char *fname);
        IFF(const string &fname);
	IFF(const IFF &i) : header(i.header),object_list(i.object_list)
		{  }
	IFF operator=(const IFF &i)
		{
		header=i.header;
		object_list=i.object_list;
		return *this;
		}
        ~IFF();

        // char *read_object(int objnum,uint32 &length);
        virtual int     number_of_objects(const char *) { return object_list.size(); };
        virtual int     retrieve(int objnum,char **,size_t *len); // To a memory block
        virtual int     retrieve(int objnum,const char *);       // To a file
private:
        IFF(); // No default constructor
        void IndexIFFFile(void);
        };


#endif
