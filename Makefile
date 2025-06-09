

all: push2talk

push2talk: push2talk.o
	gcc -o $@ $^

push2talk.o: push2talk.c
	gcc -o $@ -c $^

clean:
	rm -f push2talk push2talk.o

install: push2talk
	install -o root -g root -m 0755 $< /usr/local/bin/push2talk
	@echo ''
	@echo ''
	@echo '    Now run `make xdg-enable` to configure auto-starting the daemon'
	@echo ''
	@echo '    If you want to make modifications - like which key is monitored or which command'
	@echo '    is executed - edit the push2talk.desktop file beforehand.'
	@echo ''

xdg-enable: push2talk.desktop
	@if [ "$$UID" -lt 1000 ]; then echo ''; echo 'Please execute this as a regular user!'; echo ''; false; fi
	install -m 0644 $< $$HOME/.config/autostart/



