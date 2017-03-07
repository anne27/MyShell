HellShell: 2015014_Shell1.c
	gcc -o HellShell 2015014_Shell1.c
	gnome-terminal -e ./HellShell

clean:
	rm -f HellShell
