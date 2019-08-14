DIRS	=	1-basics 2-lighting

# -----------------------------------------------

.SUFFIXES:

.PHONY: all release debug clean

all:
	@for dir in $(DIRS); do $(MAKE) all --no-print-directory -C $$dir; done

release:
	@for dir in $(DIRS); do $(MAKE) release --no-print-directory -C $$dir; done

debug:
	@for dir in $(DIRS); do $(MAKE) debug --no-print-directory -C $$dir; done

clean:
	@for dir in $(DIRS); do $(MAKE) clean --no-print-directory -C $$dir; done
