DIRS	=	1-basics 2-lighting 3-model
LIBS    =   lib/glad lib/stb_image

# -----------------------------------------------

.SUFFIXES:

.PHONY: all libs release debug clean mrproper

all:
	@for dir in $(DIRS); do $(MAKE) all --no-print-directory -C $$dir; done

libs:
	@for dir in $(LIBS); do $(MAKE) --no-print-directory -C $$dir; done

release:
	@for dir in $(DIRS); do $(MAKE) release --no-print-directory -C $$dir; done

debug:
	@for dir in $(DIRS); do $(MAKE) debug --no-print-directory -C $$dir; done

clean:
	@for dir in $(DIRS); do $(MAKE) clean --no-print-directory -C $$dir; done

mrproper: clean
	@for dir in $(LIBS); do $(MAKE) clean --no-print-directory -C $$dir; done
