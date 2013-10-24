#include "Meeting.h"
#include "Room.h"
#include "Ordered_container.h"
#include "Person.h"
#include "p1_globals.h"
#include "Utility.h"

#include <assert.h>
#include <stdio.h>

typedef enum {BAD_COMMAND, DUP_MEETING, DUP_PARTICIPANT, DUP_PERSON,
              DUP_ROOM, INVALID_FILE_DATA, IS_NOT_PARTICIPANT, IS_PARTICIPANT,
              MEETINGS_EXIST, NO_MEETING, NO_PERSON, NO_ROOM, OPEN_FILE,
              READ_INT, OUT_OF_RANGE_ROOM, OUT_OF_RANGE_TIME} Error_e;

void add_meeting(struct Ordered_container* rooms_c);
void add_participant(struct Ordered_container* rooms_c,
                     const struct Ordered_container* persons_c);
void add_person(struct Ordered_container* rooms_c);
void add_room(struct Ordered_container* rooms_c);

void clear_input_line(void);

int comp_func_room(const void* room_ptr1, const void* room_ptr2);
int comp_func_room_arg(const void* room_number_ptr, const void* room_ptr);

void delete_person(const struct Ordered_container* rooms_c,
                   struct Ordered_container* persons_c);
void delete_room(struct Ordered_container* rooms_c);
void delete_room_all(struct Ordered_container* rooms_c);
void delete_meeting(struct Ordered_container* rooms_c);
void delete_participant(struct Ordered_container* rooms_c,
                        const struct Ordered_container* persons_c);
void delete_meeting_all(struct Ordered_container* rooms_c);
void delete_person_all(const struct Ordered_container* rooms_c,
                       struct Ordered_container* persons_c);
void delete_all(struct Ordered_container* rooms_c,
                struct Ordered_container* persons_c);
void delete_all_no_print(struct Ordered_container* rooms_c,
                         struct Ordered_container* persons_c);

void* find_data_ptr(const struct Ordered_container* c_ptr,
                    const void* arg_ptr,
                    OC_find_item_arg_fp_t comp_func_arg);

void handle_error(Error_e error);

int is_room_empty(void* room_ptr);
int is_person_in_room(void* room_ptr, void* person_ptr);
int is_person_in_meeting(void* meeting_ptr, void* person_ptr);

void load_data(struct Ordered_container* rooms_c,
               struct Ordered_container* persons_c);
int load_persons(FILE* infile,
                  struct Ordered_container* persons_c);
int load_rooms(FILE* infile,
                struct Ordered_container* rooms_c,
                const struct Ordered_container* persons_c);

void print_meeting(const struct Ordered_container* rooms_c);
void print_meeting_all(const struct Ordered_container* rooms_c);
void print_memory(const struct Ordered_container* rooms_c,
                  const struct Ordered_container* persons_c);
void print_person(const struct Ordered_container* persons_c);
void print_person_all(const struct Ordered_container* persons_c);
void print_room(const struct Ordered_container* rooms_c);
void print_room_wrapper(void* room_ptr);

int read_meeting(const struct Ordered_container* rooms_c,
                 struct Room** room_ptr,
                 int* time);
int read_person(char* firstname, char* lastname, char* phoneno);
int read_person_lastname(char* lastname);
int read_room(int* room_number);
struct Meeting* read_and_find_meeting(const struct Ordered_container* rooms_c);
struct Meeting* read_and_find_meeting_and_room(const struct Ordered_container* rooms_c,
                                               struct Room** meeting_room_ptr);
struct Person* read_and_find_person(const struct Ordered_container* persons_c);
struct Person* read_and_find_person_lastname(const struct Ordered_container* persons_c);
struct Room* read_and_find_room(const struct Ordered_container* rooms_c);

int read_room_number(void);
int read_time(void);

void reschedule_meeting(const struct Ordered_container* rooms_c);

void save_data(const struct Ordered_container* rooms_c,
               const struct Ordered_container* persons_c);

void wrapper_save_person(void* person_ptr, void* outfile);
void wrapper_save_room(void* room_ptr, void* outfile);
void wrapper_room_clear(void* room_ptr);
void wrapper_person_destroy(void* person_ptr);
void wrapper_room_destroy(void* person_ptr);

int main() {
  struct Ordered_container* rooms_c = OC_create_container(comp_func_room);
  struct Ordered_container* persons_c = OC_create_container(comp_func_person);

  char first_letter;
  char second_letter;

  while (1) {
    printf("\nEnter command: ");
    scanf(" %c", &first_letter);
    scanf(" %c", &second_letter);

    switch (first_letter) {
      case 'p': {
        switch (second_letter) {
          case 'i':
            print_person(persons_c);
            break;
          case 'r':
            print_room(rooms_c);
            break;
          case 'm':
            print_meeting(rooms_c);
            break;
          case 's':
            print_meeting_all(rooms_c);
            break;
          case 'g':
            print_person_all(persons_c);
            break;
          case 'a':
            print_memory(rooms_c, persons_c);
            break;
          default:
            handle_error(BAD_COMMAND);
            break;
        }
        break;
      }
      case 'a': {
        switch (second_letter) {
          case 'i':
            add_person(persons_c);
            break;
          case 'r':
            add_room(rooms_c);
            break;
          case 'm':
            add_meeting(rooms_c);
            break;
          case 'p':
            add_participant(rooms_c, persons_c);
            break;
          default:
            handle_error(BAD_COMMAND);
            break;
        }
        break;
      }
      case 'r': {
        switch (second_letter) {
          case 'm':
            reschedule_meeting(rooms_c);
            break;
          default:
            handle_error(BAD_COMMAND);
            break;
        }
        break;
      }
      case 'd': {
        switch (second_letter) {
          case 'i':
            delete_person(rooms_c, persons_c);
            break;
          case 'r':
            delete_room(rooms_c);
            break;
          case 'm':
            delete_meeting(rooms_c);
            break;
          case 'p':
            delete_participant(rooms_c, persons_c);
            break;
          case 's':
            delete_meeting_all(rooms_c);
            break;
          case 'g':
            delete_person_all(rooms_c, persons_c);
            break;
          case 'a':
            delete_all(rooms_c, persons_c);
            break;
          default:
            handle_error(BAD_COMMAND);
            break;
        }
        break;
      }
      case 's': {
        switch (second_letter) {
          case 'd':
            save_data(rooms_c, persons_c);
            break;
          default:
            handle_error(BAD_COMMAND);
            break;
        }
        break;
      }
      case 'l': {
        switch (second_letter) {
          case 'd':
            load_data(rooms_c, persons_c);
            break;
          default:
            handle_error(BAD_COMMAND);
            break;
        }
        break;
      }
      case 'q': {
        switch (second_letter) {
          case 'q':
            delete_all(rooms_c, persons_c);
            OC_destroy_container(rooms_c);
            OC_destroy_container(persons_c);
            printf("Done\n");
            return 0;
          default:
            handle_error(BAD_COMMAND);
            break;
        }
        break;
      }
      default:
        handle_error(BAD_COMMAND);
        break;
    }
  }

  return 0;
}

void add_meeting(struct Ordered_container* rooms_c)
{
  struct Meeting* meeting_ptr;
  struct Room* room_ptr;
  int time;
  char topic[MAX_STR_LENGTH + 1];

  if (read_meeting(rooms_c, &room_ptr, &time) != 0) {
    return;
  } else if (find_Room_Meeting(room_ptr, time)) {
    handle_error(DUP_MEETING);
    return;
  }
 
  /* The meeting is good to go. Read in the topic. */
  read_string(stdin, topic);

  /* Create the meeting object and insert it into the room. */
  meeting_ptr = create_Meeting(time, topic);
  add_Room_Meeting(room_ptr, meeting_ptr);

  printf("Meeting added at %d\n", time);
}

void add_participant(struct Ordered_container* rooms_c,
                     const struct Ordered_container* persons_c)
{
  struct Meeting* meeting_ptr;
  struct Person* person_ptr;

  if (!(meeting_ptr = read_and_find_meeting(rooms_c))) {
    return;
  } else if (!(person_ptr = read_and_find_person_lastname(persons_c))) {
    return;
  } else if (add_Meeting_participant(meeting_ptr, person_ptr)) {
    handle_error(DUP_PARTICIPANT);
    return;
  }

  printf("Participant %s added\n", get_Person_lastname(person_ptr));
}

void add_person(struct Ordered_container* persons_c)
{
  struct Person* person_ptr;
  char firstname[MAX_STR_LENGTH + 1];
  char lastname[MAX_STR_LENGTH + 1];
  char phoneno[MAX_STR_LENGTH + 1];

  if (read_person(firstname, lastname, phoneno) != 0) {
    return;
  } else if (find_data_ptr(persons_c, lastname, comp_func_person_arg)) {
    handle_error(DUP_PERSON);
    return;
  }

  person_ptr = create_Person(firstname, lastname, phoneno);
  OC_insert(persons_c, (void *)person_ptr);

  printf("Person %s added\n", lastname);
}

void add_room(struct Ordered_container* rooms_c)
{
  struct Room* room_ptr;
  int room_number;

  if (read_room(&room_number) != 0) {
    return;
  } else if (find_data_ptr(rooms_c, &room_number, comp_func_room_arg)) {
    handle_error(DUP_ROOM);
    return;
  }

  room_ptr = create_Room(room_number);
  OC_insert(rooms_c, (void *)room_ptr);

  printf("Room %d added\n", room_number);
}

int comp_func_room(const void* room_ptr1, const void* room_ptr2)
{
  return get_Room_number((struct Room*)room_ptr1) -
         get_Room_number((struct Room*)room_ptr2);
}

int comp_func_room_arg(const void* room_number_ptr, const void* room_ptr)
{
  return *((int *)room_number_ptr) - get_Room_number((struct Room*)room_ptr);
}

void delete_person(const struct Ordered_container* rooms_c,
                   struct Ordered_container* persons_c)
{
  struct Person* person_ptr;
  void* item_ptr;

  if (!(person_ptr = read_and_find_person_lastname(persons_c))) {
    return;
  } else if (OC_apply_if_arg(rooms_c, is_person_in_room, person_ptr) != 0) {
    handle_error(IS_PARTICIPANT);
    return;
  }

  printf("Person %s deleted\n", get_Person_lastname(person_ptr));

  item_ptr = OC_find_item(persons_c, person_ptr);

  assert(item_ptr);

  OC_delete_item(persons_c, item_ptr);
  destroy_Person(person_ptr);
}

int is_person_in_room(void* room_ptr, void* person_ptr)
{
 return OC_apply_if_arg(get_Room_Meetings((struct Room*)room_ptr),
                        is_person_in_meeting,
                        person_ptr);
}

int is_person_in_meeting(void* meeting_ptr, void* person_ptr)
{
  return is_Meeting_participant_present((struct Meeting*)meeting_ptr,
                                        (struct Person*)person_ptr);
}

void delete_room(struct Ordered_container* rooms_c)
{
  struct Room* room_ptr;
  void* item_ptr;

  if (!(room_ptr = read_and_find_room(rooms_c))) {
    return;
  }

  item_ptr = OC_find_item(rooms_c, room_ptr);
  OC_delete_item(rooms_c, item_ptr);

  printf("Room %d deleted\n", get_Room_number(room_ptr));

  clear_Room(room_ptr);
  destroy_Room(room_ptr);
}

void delete_room_all(struct Ordered_container* rooms_c)
{
  OC_apply(rooms_c, wrapper_room_destroy);
  OC_clear(rooms_c);

  printf("All rooms deleted\n");
}

void wrapper_room_destroy(void* room_ptr)
{
  destroy_Room((struct Room*)room_ptr);
}
  
void delete_meeting(struct Ordered_container* rooms_c)
{
  struct Meeting* meeting_ptr;
  struct Room* room_ptr;

  if (!(meeting_ptr = read_and_find_meeting_and_room(rooms_c, &room_ptr))) {
    return;
  }

  printf("Meeting at %d deleted\n", get_Meeting_time(meeting_ptr));

  remove_Room_Meeting(room_ptr, meeting_ptr);
  destroy_Meeting(meeting_ptr);
}

void delete_participant(struct Ordered_container* rooms_c,
                        const struct Ordered_container* persons_c)
{
  struct Meeting* meeting_ptr;
  struct Person* person_ptr;

  if (!(meeting_ptr = read_and_find_meeting(rooms_c))) {
    return;
  } else if (!(person_ptr = read_and_find_person_lastname(persons_c))) {
    return;
  } else if (remove_Meeting_participant(meeting_ptr, person_ptr)) {
    handle_error(IS_NOT_PARTICIPANT);
    return;
  }
  printf("Participant %s deleted\n", get_Person_lastname(person_ptr));
}

void delete_meeting_all(struct Ordered_container* rooms_c)
{
  OC_apply(rooms_c, wrapper_room_clear);
  printf("All meetings deleted\n");
}

void wrapper_room_clear(void* room_ptr)
{
  clear_Room((struct Room*)room_ptr);
}

void delete_person_all(const struct Ordered_container* rooms_c,
                       struct Ordered_container* persons_c)
{
  if (OC_apply_if(rooms_c, is_room_empty)) {
    handle_error(MEETINGS_EXIST);
    return;
  }

  OC_apply(persons_c, wrapper_person_destroy);
  OC_clear(persons_c);

  printf("All persons deleted\n");
}

int is_room_empty(void* room_ptr)
{
  return !OC_empty(get_Room_Meetings((struct Room*)room_ptr));
}

void wrapper_person_destroy(void* person_ptr)
{
  destroy_Person((struct Person*)person_ptr);
}

void delete_all(struct Ordered_container* rooms_c,
                struct Ordered_container* persons_c)
{
  /* Delete meetings, then rooms, then people. */
  delete_meeting_all(rooms_c);
  delete_room_all(rooms_c);
  delete_person_all(rooms_c, persons_c);
}

void delete_all_no_print(struct Ordered_container* rooms_c,
                         struct Ordered_container* persons_c)
{
  OC_apply(rooms_c, wrapper_room_destroy);
  OC_apply(persons_c, wrapper_person_destroy);

  OC_clear(rooms_c);
  OC_clear(persons_c);
}

void handle_error(Error_e error)
{
  switch (error) {
    case BAD_COMMAND:
      printf("Unrecognized command!\n");
      break;
    case DUP_MEETING:
      printf("There is already a meeting at that time!\n");
      break;
    case DUP_PARTICIPANT:
      printf("This person is already a participant!\n");
      break;
    case DUP_PERSON:
      printf("There is already a person with this last name!\n");
      break;
    case DUP_ROOM:
      printf("There is already a room with this number!\n");
      break;
    case INVALID_FILE_DATA:
      printf("Invalid data found in file!\n");
      break;
    case IS_NOT_PARTICIPANT:
      printf("This person is not a participant in the meeting!\n");
      break;
    case IS_PARTICIPANT:
      printf("This person is a participant in a meeting!\n");
      break;
    case MEETINGS_EXIST:
      printf("Cannot clear people list unless there are no meetings!\n");
      break;
    case NO_MEETING:
      printf("No meeting at that time!\n");
      break;
    case NO_PERSON:
      printf("No person with that name!\n");
      break;
    case NO_ROOM:
      printf("No room with that number!\n");
      break;
    case OPEN_FILE:
      printf("Could not open file!\n");
      break;
    case OUT_OF_RANGE_ROOM:
      printf("Room number is not in range!\n");
      break;
    case OUT_OF_RANGE_TIME:
      printf("Time is not in range!\n");
      break;
    case READ_INT:
      printf("Could not read an integer value!\n");
      break;
  }
  
  while (getchar() != '\n');
}

void* find_data_ptr(const struct Ordered_container* c_ptr,
                    const void* arg_ptr,
                    OC_find_item_arg_fp_t comp_func_arg)
{
  void* item_ptr = OC_find_item_arg(c_ptr, arg_ptr, comp_func_arg);

  return (item_ptr) ? OC_get_data_ptr(item_ptr) : NULL;
}

void load_data(struct Ordered_container* rooms_c,
               struct Ordered_container* persons_c)
{
  FILE* infile;
  char filename[MAX_STR_LENGTH + 1];

  read_string(stdin, filename);

  if (!(infile = fopen(filename, "r"))) {
    handle_error(OPEN_FILE);
    return;
  }
  
  delete_all_no_print(rooms_c, persons_c);
  
  if (load_persons(infile, persons_c) != 0 ||
      load_rooms(infile, rooms_c, persons_c) != 0) {
    handle_error(INVALID_FILE_DATA);
    delete_all_no_print(rooms_c, persons_c);
  } else {
    printf("Data loaded\n");
  }

  fclose(infile);
}

int load_persons(FILE* infile, struct Ordered_container* persons_c)
{
  struct Person* person_ptr;
  int num_persons;

  if (fscanf(infile, "%d", &num_persons) != 1) {
    return 1;
  }

  for (; num_persons > 0; --num_persons) {
    if (!(person_ptr = load_Person(infile))) {
      return 1;
    }
    OC_insert(persons_c, person_ptr);
  }
  return 0;
}

int load_rooms(FILE* infile,
               struct Ordered_container* rooms_c,
               const struct Ordered_container* persons_c)
{
  struct Room* room_ptr;
  int num_rooms;

  if (fscanf(infile, "%d", &num_rooms) != 1) {
    return 1;
  }

  for (; num_rooms > 0; --num_rooms) {
    if (!(room_ptr = load_Room(infile, persons_c))) {
      return 1;
    }
    OC_insert(rooms_c, room_ptr);
  }
  return 0;
} 

int read_room_number()
{
  int room_number;

  if (scanf("%d", &room_number) != 1) {
    handle_error(READ_INT);
    return 0;
  } else if (room_number <= 0) {
    handle_error(OUT_OF_RANGE_ROOM);
    return 0;
  }

  return room_number;
}

int read_time()
{
  int time;
  if (scanf("%d", &time) != 1) {
    handle_error(READ_INT);
    return 0;
  } else if (time < 1 || time > 12 || (time > 5 && time < 9)) {
    handle_error(OUT_OF_RANGE_TIME);
    return 0;
  }

  return time;
}

void print_meeting(const struct Ordered_container* rooms_c)
{
  struct Meeting* meeting_ptr;

  if (!(meeting_ptr = read_and_find_meeting(rooms_c))) {
    return;
  } 

  print_Meeting(meeting_ptr);
}
 
void print_meeting_all(const struct Ordered_container* rooms_c)
{
  if (OC_get_size(rooms_c) == 0) {
    printf("List of rooms is empty\n");
    return;
  }

  printf("Information for %d rooms:\n", OC_get_size(rooms_c));
  OC_apply(rooms_c, print_room_wrapper);
}

void print_memory(const struct Ordered_container* rooms_c,
                  const struct Ordered_container* persons_c)
{
  printf("Memory allocations:\n");
  printf("C-strings: %d bytes total\n", g_string_memory);
  printf("Person structs: %d\n", OC_get_size(persons_c));
  printf("Meeting structs: %d\n", g_Meeting_memory);
  printf("Room structs: %d\n", OC_get_size(rooms_c));
  printf("Containers: %d\n", g_Container_count);
  printf("Container items in use: %d\n", g_Container_items_in_use);
  printf("Container items allocated: %d\n", g_Container_items_allocated);
}

void print_person(const struct Ordered_container* persons_c)
{
  struct Person* person_ptr;

  if (!(person_ptr = read_and_find_person_lastname(persons_c))) {
    return;
  }

  print_Person(person_ptr);
}


void print_person_all(const struct Ordered_container* persons_c)
{
  if (OC_get_size(persons_c) == 0) {
    printf("List of people is empty\n");
    return;
  }

  printf("Information for %d people:\n", OC_get_size(persons_c));
  OC_apply(persons_c, print_person_wrapper);
}

void print_room(const struct Ordered_container* rooms_c)
{
  struct Room* room_ptr;

  if (!(room_ptr = read_and_find_room(rooms_c))) {
    return;
  }

  print_Room(room_ptr);
}

void print_room_wrapper(void* room_ptr)
{
  print_Room((struct Room*)room_ptr);
}

int read_meeting(const struct Ordered_container* rooms_c,
                 struct Room** room_ptr,
                 int* time)
{
  if (!(*room_ptr = read_and_find_room(rooms_c))) {
    return 1;
  } else if (!(*time = read_time())) {
    return 1;
  }
  return 0;
}

int read_person(char* firstname, char* lastname, char* phoneno)
{
  read_string(stdin, firstname);
  read_string(stdin, lastname);
  read_string(stdin, phoneno);

  return 0;
}

int read_person_lastname(char* lastname)
{
  read_string(stdin, lastname);
  return 0;
}

int read_room(int* room_number) {
  if (!(*room_number = read_room_number())) {
    return 1;
  }
  return 0;
}

struct Meeting* read_and_find_meeting(const struct Ordered_container* rooms_c)
{
  struct Room* temp_room_ptr;
  return read_and_find_meeting_and_room(rooms_c, &temp_room_ptr);
}

struct Meeting* read_and_find_meeting_and_room(
    const struct Ordered_container* rooms_c, struct Room** meeting_room_ptr)
{
  int time;
  struct Meeting* meeting_ptr = NULL;

  if (read_meeting(rooms_c, meeting_room_ptr, &time) != 0) {
    return NULL;
  } else if (!(meeting_ptr = find_Room_Meeting(*meeting_room_ptr, time))) {
    handle_error(NO_MEETING);
    return NULL;
  }

  return meeting_ptr;
}

struct Person* read_and_find_person(const struct Ordered_container* persons_c)
{
  struct Person* person_ptr;
  char firstname[MAX_STR_LENGTH + 1];
  char lastname[MAX_STR_LENGTH + 1];
  char phoneno[MAX_STR_LENGTH + 1];

  read_person(firstname, lastname, phoneno);

  if (!(person_ptr = find_data_ptr(persons_c, lastname, comp_func_person_arg))) {
    handle_error(NO_PERSON);
    return NULL;
  }

  return person_ptr;
}

struct Person* read_and_find_person_lastname(
    const struct Ordered_container* persons_c)
{
  struct Person* person_ptr;
  char lastname[MAX_STR_LENGTH + 1];

  read_person_lastname(lastname);

  if (!(person_ptr = find_data_ptr(persons_c,
                                   lastname,
                                   comp_func_person_arg))) {
    handle_error(NO_PERSON);
    return NULL;
  } 

  return person_ptr;
}

struct Room* read_and_find_room(const struct Ordered_container* rooms_c)
{
  struct Room* room_ptr;
  int room_number;

  if (read_room(&room_number) != 0) {
    return NULL;
  } else if (!(room_ptr = find_data_ptr(rooms_c,
                                        &room_number,
                                        comp_func_room_arg))) {
    handle_error(NO_ROOM);
    return NULL;
  }

  return room_ptr;
}

void reschedule_meeting(const struct Ordered_container* rooms_c)
{
  struct Meeting* meeting_ptr;
  struct Room* old_room_ptr;
  struct Room* new_room_ptr;
  int new_time;

  if (!(meeting_ptr = read_and_find_meeting_and_room(rooms_c, &old_room_ptr))) {
    return;
  } else if (read_meeting(rooms_c, &new_room_ptr, &new_time) != 0) {
    return;
  } else if (find_Room_Meeting(new_room_ptr, new_time)) {
    handle_error(DUP_MEETING);
    return;
  }

  /* Everything is good to go. Make the switch. */
  remove_Room_Meeting(old_room_ptr, meeting_ptr);
  set_Meeting_time(meeting_ptr, new_time);
  add_Room_Meeting(new_room_ptr, meeting_ptr);

  printf("Meeting rescheduled to room %d at %d\n",
         get_Room_number(new_room_ptr),
         new_time);
}

void save_data(const struct Ordered_container* rooms_c,
               const struct Ordered_container* persons_c)
{
  FILE* outfile;
  char filename[MAX_STR_LENGTH + 1];

  read_string(stdin, filename);

  if (!(outfile = fopen(filename, "w"))) {
    handle_error(OPEN_FILE);
    return;
  }

  fprintf(outfile, "%d\n", OC_get_size(persons_c));
  OC_apply_arg(persons_c, wrapper_save_person, outfile);

  fprintf(outfile, "%d\n", OC_get_size(rooms_c));
  OC_apply_arg(rooms_c, wrapper_save_room, outfile);

  fclose(outfile);

  printf("Data saved\n");
}

void wrapper_save_person(void* person_ptr, void* outfile)
{
  save_Person((struct Person*)person_ptr, (FILE *)outfile);
}

void wrapper_save_room(void* room_ptr, void* outfile)
{
  save_Room((struct Room*)room_ptr, (FILE *)outfile);
}
