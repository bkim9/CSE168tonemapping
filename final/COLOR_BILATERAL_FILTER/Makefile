#     Copyright (c) 2006, Sylvain Paris and Fr�do Durand

#     Permission is hereby granted, free of charge, to any person
#     obtaining a copy of this software and associated documentation
#     files (the "Software"), to deal in the Software without
#     restriction, including without limitation the rights to use, copy,
#     modify, merge, publish, distribute, sublicense, and/or sell copies
#     of the Software, and to permit persons to whom the Software is
#     furnished to do so, subject to the following conditions:

#     The above copyright notice and this permission notice shall be
#     included in all copies or substantial portions of the Software.

#     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#     EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#     NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#     HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#     WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#     OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#     DEALINGS IN THE SOFTWARE.








######################################################
# NO MODIFICATION SHOULD BE NEEDED BEYOND THIS POINT #
######################################################



# General section
INCDIR	= -I. -Iinclude
LIBS	=  -lm
DEFINES = -DNO_XML
OBJECTS	= color_bilateral_filter.o 
TARGET	= color_bilateral_filter

# Command section
CC	= g++
LINK	= g++
RM	= rm -f

# General rules
default: $(OBJECTS)
	$(LINK) $(OBJECTS) $(LIBS) -o $(TARGET)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(TARGET)

# File rules
color_bilateral_filter.o: color_bilateral_filter.cpp
	$(CC) color_bilateral_filter.cpp -c -o color_bilateral_filter.o $(INCDIR) $(DEFINES)

# g++ color_bilateral_filter.o   -lm -o color_bilateral_filter