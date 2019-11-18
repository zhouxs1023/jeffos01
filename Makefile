#Cross-compilation prefix 
#PREFIX = i386-unknown-elf-
#POSTFIX = 386

CC = $(PREFIX)gcc$(POSTFIX)
#CC = lcc -N -I/usr/local/lib/lcc-4.0/include
LD = $(PREFIX)ld$(POSTFIX)
NM = $(PREFIX)nm$(POSTFIX)
ST = $(PREFIX)strip$(POSTFIX)
AR = $(PREFIX)ar$(POSTFIX)
CF = -O2
#-funroll-loops -fomit-frame-pointer


ENV = CC="$(CC)" LD="$(LD)" NM="$(NM)" ST="$(ST)" CF="$(CF)"

all: boot kernel lib user

boot:: 
	@echo "" 
	@echo "--- boot ----------------"
	@cd boot ; $(MAKE) $(ENV)

kernel::
	@echo ""
	@echo "--- kernel --------------"
	@cd kernel ; $(MAKE) $(ENV)

lib::
	@echo ""
	@echo "--- lib -----------------"
	@cd lib ; $(MAKE) $(ENV)

user::
	@echo ""
	@echo "--- user --------------"
	@cd user ; $(MAKE) $(ENV)

clean:
	@cd boot ; $(MAKE) clean
	@cd kernel ; $(MAKE) clean
	@cd lib ; $(MAKE) clean
	@cd user ; $(MAKE) clean

