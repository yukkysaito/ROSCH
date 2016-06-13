.PHONY: all
all:
	./script/all

.PHONY: clean
clean:
	rm -f $(TARGET).o $(TARGET).a $(TARGET).so* *~
	rm -f $(TARGET_GPU).o $(TARGET_GPU).a $(TARGET_GPU).so* *~
	rm -f $(NVRM_OBJS)

.PHONY: install
install:
	./script/install

.PHONY: uninstall
uninstall:
	./script/uninstall
