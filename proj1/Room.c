#include "Meeting.h"
#include "Ordered_container.h"
#include "Room.h"
#include "Utility.h"

#include <stdio.h>
#include <stdlib.h>

/* a Room contains a container of meetings and a room number */
struct Room {
    /* a container of pointers to Meeting objects */
	struct Ordered_container* meetings;
	int number;
};

int meeting_comp_func(const void* meeting_ptr1, const void* meeting_ptr2);
int meeting_arg_comp_func(const void* time_ptr, const void* meeting_ptr);
static int normalize_meeting_time(int original_time);

void destroy_meeting_wrapper(void* meeting_ptr);
void print_meeting_wrapper(void* meeting_ptr);
void save_meeting_wrapper(void* meeting_ptr, void* outfile);

struct Room* create_Room(int number)
{
  struct Room* new_room = safe_malloc(sizeof(struct Room));

  new_room->number = number;
  new_room->meetings = OC_create_container(meeting_comp_func);

  return new_room;
}

void destroy_Room(struct Room* room_ptr)
{
  /* Destroy all of the meeting objects. */
  clear_Room(room_ptr);

  /* Destroy the container that held the meeting objects. */
  OC_destroy_container(room_ptr->meetings);

  /* Free the room struct pointer. */
  free(room_ptr);
}

int get_Room_number(const struct Room* room_ptr)
{
  return room_ptr->number;
}

int add_Room_Meeting(struct Room* room_ptr, const struct Meeting* meeting_ptr)
{
  /* If we find a meeting already at that time in the room, don't add it. */
  if (OC_find_item(room_ptr->meetings, meeting_ptr)) {
    return 1;
  }

  /* There is no meeting at the proposed time yet, add it. */
  OC_insert(room_ptr->meetings, (void *)meeting_ptr);
  return 0;
}

struct Meeting* find_Room_Meeting(const struct Room* room_ptr, int time)
{  
  /* Find the item ptr that corresponds to the meeting at the specified time. */
  void* item_ptr =
      OC_find_item_arg(room_ptr->meetings, &time, meeting_arg_comp_func);

  return (item_ptr) ? OC_get_data_ptr(item_ptr) : NULL;
}

int remove_Room_Meeting(struct Room* room_ptr,
                        const struct Meeting* meeting_ptr)
{
  void* item_ptr = OC_find_item(room_ptr->meetings, meeting_ptr);

  if (!item_ptr) {
    return 1;
  }

  OC_delete_item(room_ptr->meetings, item_ptr);
  return 0;
}

void clear_Room(struct Room* room_ptr) {
  OC_apply(room_ptr->meetings, destroy_meeting_wrapper);
  OC_clear(room_ptr->meetings);
}

const struct Ordered_container* get_Room_Meetings(const struct Room* room_ptr)
{
  return room_ptr->meetings;
}

void print_Room(const struct Room* room_ptr)
{
  printf("--- Room %d ---\n", room_ptr->number);

  if (OC_get_size(room_ptr->meetings) == 0) {
    printf("No meetings are scheduled\n");
  } else {
    OC_apply(room_ptr->meetings, print_meeting_wrapper);
  }
}

void save_Room(const struct Room* room_ptr, FILE* outfile) {
  fprintf(outfile, "%d %d\n", room_ptr->number,
                              OC_get_size(room_ptr->meetings));

  OC_apply_arg(room_ptr->meetings, save_meeting_wrapper, outfile);
}

struct Room* load_Room(FILE* infile, const struct Ordered_container* people)
{
  struct Room* new_room;
  int room_number;
  int num_meetings;

  if (fscanf(infile, "%d", &room_number) != 1 ||
      fscanf(infile, "%d", &num_meetings) != 1) {
    return NULL;
  }

  /* Looks like we have a valid room, allocate the space. */
  new_room = create_Room(room_number);

  /* Add all the meetings in this room. */
  while (num_meetings-- > 0) {
    struct Meeting* meeting = load_Meeting(infile, people);
    if (!meeting) {
      /* Something's gone wrong with loading the meetings. */
      destroy_Room(new_room);
      return NULL;
    }
    add_Room_Meeting(new_room, meeting);
  }

  return new_room;
}   

int meeting_comp_func(const void* data_ptr1, const void* data_ptr2)
{  
  return normalize_meeting_time(get_Meeting_time((struct Meeting*)data_ptr1)) -
         normalize_meeting_time(get_Meeting_time((struct Meeting*)data_ptr2));
}

int meeting_arg_comp_func(const void* time_ptr, const void* meeting_ptr)
{
  return normalize_meeting_time(*((int *)time_ptr)) -
         normalize_meeting_time(get_Meeting_time((struct Meeting*)meeting_ptr));
}

static int normalize_meeting_time(int original_time)
{
  if (original_time < 6) {
    return original_time + 12;
  } else {
    return original_time;
  }
}

void destroy_meeting_wrapper(void* meeting_ptr)
{
  destroy_Meeting((struct Meeting*)meeting_ptr);
}

void print_meeting_wrapper(void* meeting_ptr)
{
  print_Meeting((struct Meeting*)meeting_ptr);
}

void save_meeting_wrapper(void* meeting_ptr, void* outfile)
{
  save_Meeting((struct Meeting*)meeting_ptr, (FILE *)outfile);
}
