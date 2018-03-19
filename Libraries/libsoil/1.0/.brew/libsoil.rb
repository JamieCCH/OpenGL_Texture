require 'formula'

class Libsoil < Formula
  version '1.0'
  homepage 'http://www.lonesock.net/soil.html'
  url "https://github.com/childhood/libSOIL.git"
  head "https://https://github.com/childhood/libSOIL"

  def patches
    DATA
  end

  def install
    system "make", "brewable"
    prefix.install Dir["build/*"]
  end
end

__END__
diff --git a/Makefile b/Makefile
index 6844aa6..ad0b76c 100755
--- a/Makefile
+++ b/Makefile
@@ -1,7 +1,7 @@
 MAKE = make
 CC = gcc
-INSTALL_FILE = install -p -o root -g admin -m 644
-INSTALL_DIR = install -o root -g admin -d
+INSTALL_FILE = install -p -m 644
+INSTALL_DIR = install -d
 LN = ln -s
 RM = rm -fv
 CFLAGS += -c -O2 -Wall
@@ -17,8 +17,9 @@ HFILES = SOIL.h image_DXT.h image_helper.h \
   stbi_DDS_aug.h stbi_DDS_aug_c.h stb_image_aug.h
 AFILE = libSOIL.a
 DYLIBFILE = libSOIL.dylib
-INCLUDEDIR = opt/local/include/SOIL
-LIBDIR = opt/local/lib
+INCLUDEDIR = include/SOIL
+LIBDIR = lib
+DESTDIR = /$(HOMEBREW_PREFIX)
 
 all: $(OFILES) lib
 
@@ -32,6 +33,13 @@ lib: $(OFILES)
 	gcc -dynamiclib -o $(DYLIBFILE) $(OFILES) $(LDFLAGS)\
 	 	-install_name $(DESTDIR)/$(LIBDIR)/$(DYLIBFILE)
 
+brewable: all
+	mkdir -p build/include/SOIL
+	mkdir build/lib
+	cp $(HFILES) build/include/SOIL/
+	cp $(AFILE) build/lib/
+	cp $(DYLIBFILE) build/lib/
+
 install:
 	$(INSTALL_DIR) $(DESTDIR)/$(INCLUDEDIR)
 	$(INSTALL_FILE) $(HFILES) $(DESTDIR)/$(INCLUDEDIR)

