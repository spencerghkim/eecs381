#include "Meeting.h"
#include "Person.h"
#include "p1_globals.h"
#include "Ordered_container.h"
#include "Utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* a Meeting contains a time, a topic, and a container of participants */
struct Meeting {
	char* topic;
	int time;
    /* a container of pointers to struct Person objects */
	struct Ordered_container* participants;
};

void save_participants(void* person_ptr, void* file_ptr);

struct Meeting* create_Meeting(int time, const char* topic)
{
  struct Meeting* new_Meeting = safe_malloc(sizeof(struct Meeting));

  new_Meeting->topic = alloc_char(strlen(topic) + 1);
  strcpy(new_Meeting->topic, topic);

  new_Meeting->time = time;
  new_Meeting->participants = OC_create_container(comp_func_person);

  g_Meeting_memory++;

  return new_Meeting;
}

void destroy_Meeting(struct Meeting* meeting_ptr)
{
  OC_destroy_container(meeting_ptr->participants);
  free_char(meeting_ptr->topic);
  free(meeting_ptr);

  g_Meeting_memory--;
}

int get_Meeting_time(const struct Meeting* meeting_ptr)
{
  return meeting_ptr->time;
}

void set_Meeting_time(struct Meeting* meeting_ptr, int time)
{
  meeting_ptr->time = time;
}

int add_Meeting_participant(struct Meeting* meeting_ptr,
                            const struct Person* person_ptr)
{
  if (OC_find_item(meeting_ptr->participants, person_ptr)) {
    return 1;
  }

  OC_insert(meeting_ptr->participants, (void *)person_ptr);
  return 0;
}

int is_Meeting_participant_present(const struct Meeting* meeting_ptr,
                                   const struct Person* person_ptr)
{
  return (OC_find_item(meeting_ptr->participants, person_ptr) != NULL);
}

int remove_Meeting_participant(struct Meeting* meeting_ptr,
                               const struct Person* person_ptr)
{
  void* item_ptr = OC_find_item(meeting_ptr->participants, person_ptr);

  if (!item_ptr) {
    return 1;
  }

  OC_delete_item(meeting_ptr->participants, item_ptr);
  return 0;
}

void print_Meeting(const struct Meeting* meeting_ptr)
{
  int num_participants = OC_get_size(meeting_ptr->participants);

  printf("Meeting time: %d, Topic: %s\nParticipants:", meeting_ptr->time,
                                                       meeting_ptr->topic);
  if (num_participants == 0) {
    printf(" None\n");
  } else {
    printf("\n");
  }

  OC_apply(meeting_ptr->participants, wrapper_person_print);
}

void save_Meeting(const struct Meeting* meeting_ptr, FILE* outfile)
{
  fprintf(outfile, "%d %s %d\n", meeting_ptr->time,
                                 meeting_ptr->topic,
                                 OC_get_size(meeting_ptr->participants));

  OC_apply_arg(meeting_ptr->participants, save_participants, outfile);
}

struct Meeting* load_Meeting(FILE* input_file,
                             const struct Ordered_container* people)
{
  struct Meeting* new_Meeting_ptr;
  void* person_item_ptr;
  int time;
  char buffer[MAX_STR_LENGTH];
  int num_participants;

  if (fscanf(input_file, "%d", &time) != 1 ||
      read_string(input_file, buffer) != 1) {
    return NULL;
  }

  /* Successful read so far, let's create the Meeting. */
  new_Meeting_ptr = create_Meeting(time, buffer);

  if (fscanf(input_file, "%d", &num_participants) != 1) {
    destroy_Meeting(new_Meeting_ptr);
    return NULL;
  }

  /* We have the number of participants, let's read them all in. */
  while (num_participants-- > 0) {
    if (read_string(input_file, buffer) != 1 ||
        !(person_item_ptr = OC_find_item_arg(people,
                                             buffer,
                                             comp_func_person_arg))) {
      destroy_Meeting(new_Meeting_ptr);
      return NULL;
    }
    add_Meeting_participant(new_Meeting_ptr, OC_get_data_ptr(person_item_ptr));
  }

  return new_Meeting_ptr;
}   

void save_participants(void* person_ptr, void* file_ptr)
{
  fprintf(((FILE *)file_ptr),
          "%s\n",
          get_Person_lastname((struct Person*)person_ptr));
}
