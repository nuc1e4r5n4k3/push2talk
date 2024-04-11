#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <linux/input.h>

#define INPUT_PATH "/dev/input/by-id"


struct keyboards {
    size_t total;
    struct pollfd keyboards[1];
};


#define MAX_KEYBOARDS 32

struct keyboards *open_keyboards()
{
    DIR *dir = opendir(INPUT_PATH);
    struct dirent *entry;
    int total = 0;
    int files[MAX_KEYBOARDS];
    struct keyboards *result = NULL;

    while ((entry = readdir(dir)))
    {
	size_t namesize = strlen(entry->d_name);
	char path[512];

	if (!strcmp(entry->d_name + namesize - 4, "-kbd") && total < MAX_KEYBOARDS)
	{
	    strcpy(path, INPUT_PATH "/");
	    strcat(path, entry->d_name);

	    files[total++] = open(path, O_RDONLY);
	}
    }

    closedir(dir);

    result = malloc(sizeof(struct keyboards) + sizeof(struct pollfd *) * (total - 1));

    result->total = total;

    int i;
    for (i = 0; i < total; i++)
    {
	result->keyboards[i].fd = files[i];
	result->keyboards[i].events = POLLIN;
    }

    return result;
}


void runcmd(const char *command)
{
    system(command);
}

int main(int argc, char **argv)
{
    struct keyboards *keyboards = open_keyboards();

    if (argc != 4)
    {
	printf("Usage: %s <keycode> <command-on-press> <command-on-release>\n", argv[0]);
	return 1;
    }

    int keycode = atoi(argv[1]);
    const char *press_command = argv[2];
    const char *release_command = argv[3];
    int i;
    struct input_event event;

    for (;;)
    {
	for (i = 0; i < keyboards->total; i++)
	    keyboards->keyboards[i].revents = 0;

	poll(keyboards->keyboards, keyboards->total, -1);

	for (i = 0; i < keyboards->total; i++)
	{
	    if (keyboards->keyboards[i].revents == POLLIN)
	    {
		if (read(keyboards->keyboards[i].fd, &event, sizeof(event)) == sizeof(event))
		{
		    if (event.type == EV_KEY && event.code == keycode)
		    {
			if (event.value == 1)
			    runcmd(press_command);
			else if (event.value == 0)
			    runcmd(release_command);
		    }
		}
	    }
	}
    }

    return 0;
}
