#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>

#define SERVER_IP "192.168.29.120"
#define PORT 8080
#define DAILY 1
#define WEEKLY 2
#define MONTHLY 3

int debug = 0; // Global variable to hold debug level

void setDebugLevel(int argc, char * argv[]) {
  if (argc <= 3) {
    if (argc == 1) {
      printf("\n");
    } else if (argc == 2) {
      if (!strcmp(argv[1], "-v") == 0) {
        printf("Invalid input! Enter -v\n");
        exit(1);
      } else {
        debug = 2;
        printf("Debug level set to: %d\n", debug);
      }
    } else if (argc == 3) {
      if (!strcmp(argv[1], "-v") && (strcmp(argv[2], "1") == 0 || strcmp(argv[2], "2") == 0 || strcmp(argv[2], "3") == 0)) {
        debug = atoi(argv[2]);
        printf("Debug level set to: %d\n", debug);
      } else {
        printf("Invalid input!\n Input format is -v 1 or -v 2 or -v 3 only\n");
        exit(1);
      }
    } else {
      debug = 2;
      printf("Debug level set to: %d\n", debug);
    }
  }
}

void debug_log(int level,
  const char * message) {
  if (debug >= level) {
    printf("Debug[%d]: %s\n", level, message);
  }
}

typedef struct {
  int restaurant_id;
  char * username;
  char * password;
}
AdminCredentials;

AdminCredentials adminCredentials[] = {
  {
    1,
    "admin1",
    "password1"
  },
  {
    2,
    "admin2",
    "password2"
  },
  // Add more restaurant-admin mappings as needed
};

int numAdminCredentials = sizeof(adminCredentials) / sizeof(adminCredentials[0]);

void send_message(int socket, char * message) {
  debug_log(2, "Send Message\n");
  send(socket, message, strlen(message), 0);
}

void receive_message(int socket, char * buffer) {
  debug_log(2, "Receive Message\n");
  memset(buffer, 0, sizeof(buffer));
  recv(socket, buffer, sizeof(buffer), 0);
}

void display_restaurants(int client_socket) {
  debug_log(2, "Display Restaurants\n");
  char buffer[1024];
  int bytes_received;
  debug_log(3, "Variables created for display restaurants function\n");
  int i = 0;

  // Receive order history data from the server
  while (1) {
     debug_log(3, "Receive order history data from the server\n");
    // Clear the buffer for the next iteration
    memset(buffer, 0, sizeof(buffer));
    debug_log(3, "Clear of Buffer\n");
    // Receive data from the server
    debug_log(3, "Receive data from the server\n");
    char ack[] = "ACK";
    send(client_socket, ack, strlen(ack), 0);
    sleep(1);

    bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    debug_log(3, "Received restaurant details from server\n");
    if (bytes_received < 0) {
      debug_log(3, "Error receiving data for display restaurants function\n");
      perror("Error receiving data\n");
      exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {
      // Server closed the connection
      debug_log(3, "Server connection closed for display restaurants function\n");
      break;

    }

    // Check if it's the end of data message
    debug_log(3, "Checking for the end message from server\n");
    if (strncmp(buffer, "END", 3) == 0) {
      debug_log(3, "Waiting for end message\n");
      break;

    }

    // Print received restaurants data
    printf("%s", buffer);
    debug_log(3, "Print received restaurants data\n");

  }

  printf("Restaurants displayed.\n");
}

void add_item(int client_socket) {
  debug_log(2, "Entering to add item function\n");
  int num_items;
  printf("Enter the number of items to add: ");
  scanf("%d", & num_items);
  // Send the number of items to the server
  send(client_socket, & num_items, sizeof(num_items), 0);

  debug_log(3, "Sending number of items to server\n");
  // Loop to send each item detail to the server
  for (int i = 0; i < num_items; i++) {

    char item_name[100];
    float price;

    printf("Enter Item Details for Item %d:\n", i + 1);
    printf("Item Name: ");
    scanf("%s", item_name);
    debug_log(3, "Send of item details toserver\n");
    // Send item name to the server
    send(client_socket, item_name, sizeof(item_name), 0);

    printf("Price: ");
    scanf("%f", & price);

    // Send price to the server
    send(client_socket, & price, sizeof(price), 0);

  }
}

void remove_item(int client_socket) {
  debug_log(2, "Entering to remove item function\n");
  int item_id;
  char message[2048];
  debug_log(3, "Variables created for remove item function\n");
  // Read item ID from the user
  printf("Enter item ID to be removed: ");
  scanf("%d", & item_id);

  // Send item ID to the server
  send(client_socket, & item_id, sizeof(item_id), 0);

  debug_log(3, "Sending item_id to server to remove item\n");
  // Receive response from the server
  recv(client_socket, message, sizeof(message) - 1, 0);

  debug_log(3, "Receive response from server that item removed\n");
  printf("Server response: %s\n", message);
}

void update_item(int client_socket) {
  debug_log(2, "Entering to update item function\n");
  int item_id;
  char item_name[100];
  float price;
  char message[2048];
  debug_log(3, "Created variables for update item function\n");
  // Read updated item details from the user
  printf("Enter item ID to be updated: ");
  scanf("%d", & item_id);

  printf("Enter updated item name: ");
  scanf("%s", item_name);

  printf("Enter updated price: ");
  scanf("%f", & price);

  // Send updated item details to the server
  send(client_socket, & item_id, sizeof(item_id), 0);

  send(client_socket, item_name, sizeof(item_name), 0);

  send(client_socket, & price, sizeof(price), 0);

  debug_log(3, "Sending item details to server for update\n");
  // Receive response from the server
  recv(client_socket, message, sizeof(message) - 1, 0);
  debug_log(3, "Receive response from server that item updated\n");
  printf(" %s\n", message);

}

void display_order_history(int client_socket) {
  debug_log(2, "Entering to order history function\n");
  char buffer[1024];
  int bytes_received;
  int time_interval;
  debug_log(3, "Variables created for order history function\n");
  printf("Select time interval:\n");
  printf("1. Daily\n");
  printf("2. Weekly\n");
  printf("3. Monthly\n");
  printf("Enter your choice: ");
  scanf("%d", & time_interval);

  // Send request for order history with the desired time interval
  send(client_socket, & time_interval, sizeof(time_interval), 0);

  debug_log(3, "Sending request for order history with the desired time interval\n");
  // Receive order history data from the server
  debug_log(3, "Receive response from server to display order history\n");
  while (1) {

    // Clear the buffer for the next iteration
    memset(buffer, 0, sizeof(buffer));
    // Receive data from the server
    debug_log(3,"Clear the buffer and receive data from the server\n");
    char ack[] = "ACK";
    send(client_socket, ack, strlen(ack), 0);
    sleep(1);
    bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received < 0) {
      debug_log(3, "Error receiving data for display order history function\n");
      perror("Error receiving data");
      exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {

      debug_log(3, "Server connection closed for display order history function\n");
      // Server closed the connection
      break;
    }

    // Check if it's the end of data message
    if (strncmp(buffer, "END", 3) == 0) {
      debug_log(3, "Waiting for end message for display order history function\n");

      break;
    }

    // Print received order history data
    printf("%s", buffer);

    debug_log(3, "Printing order history\n");

  }

  printf("Order history displayed.\n");
}

void receive_menu(int client_socket) {
  debug_log(2, "Entering to receive menu function\n");
  char buffer[1024];
  int bytes_received;
  debug_log(3, "Variables created for receive menu function\n");
  // Receive menu data from the server

  while (1) {

    // Clear the buffer for the next iteration
    memset(buffer, 0, sizeof(buffer));
    debug_log(3, "Clearing the buffer in receive menu function\n");
    // Receive data from the server
    char ack[] = "ACK";
    send(client_socket, ack, strlen(ack), 0);
    sleep(1);
    bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    debug_log(3, "Receiving data from server for the receive menu to the customer\n");

    if (bytes_received < 0) {
      debug_log(3, "Error receiving data for the receive menu to the customer\n");
      perror("Error receiving data");
      exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {
      debug_log(3, "Server connection closed for the receive menu to the customer\n");
      // Server closed the connection
      break;

    }

    // Check if it's the end of data message
    if (strncmp(buffer, "END", 3) == 0) {

      debug_log(3, "Waiting for end message from server for the receive menu to the customer\n");
      break;

    }

    // Print received menu data
    printf("%s", buffer);

    debug_log(3, "Printing received menu data from server\n");
  }

  printf("\nMenu displayed.\n");
}

void clear_input_buffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}

// Admin Interface
void admin_interface(int client_socket) {

  debug_log(1, "Entering to admin interface\n");
  printf("\t---------------------------------------------\n");
  printf("\tWelcome to Admin Interface\t\n");
  printf("\t---------------------------------------------\n");
  printf("\n");
  int choice;
  do {

    // Display admin menu options
    debug_log(2, "Display of admin menu options\n");
    char message[1024];
    printf("1. Add Restaurant\n");
    printf("2. Display Restaurant\n");
    printf("3. Exit\n");
    printf("\nEnter your choice: ");
    fgets(message, sizeof(message), stdin);

    // Check if the input is numeric
    if (isdigit(message[0])) {
      debug_log(3, "Sending the choice to server\n");
      send(client_socket, message, strlen(message), 0);

      choice = atoi(message);
      char name[100];
      char address[100];
      char contact_number[20];
      switch (choice) {
      case 1:
        debug_log(3, "Selection of adding restaurants choice\n");
        debug_log(3, "Sending restaurant details to server\n");
        printf("Enter restaurant name: ");
        fgets(message, sizeof(message), stdin);
        send(client_socket, message, strlen(message), 0);

        printf("Enter restaurant address: ");
        fgets(message, sizeof(message), stdin);
        send(client_socket, message, strlen(message), 0);

        printf("Enter contact number: ");
        fgets(message, sizeof(message), stdin);
        send(client_socket, message, strlen(message), 0);

        // Receive response from server
        debug_log(3, "Receiving response from server\n");
        memset(message, 0, sizeof(message));
        recv(client_socket, message, sizeof(message) - 1, 0);

        printf("Server response: %s\n", message);

        break;
      case 2:
        debug_log(3, "Selection of display restaurants choice\n");
        display_restaurants(client_socket);

        break;
      case 3:
        debug_log(3, "Selection of exit choice\n");
        printf("Exiting ...\n");
        break;
      default:
        debug_log(3, "Invalid choice\n");
        printf("Invalid choice\n");

      }
    } else {
      // Input is not numeric

      printf("Invalid input. Please enter a numeric choice.\n");

    }
  } while (choice != 3);

  int restaurant_id;
  char message[1024];
  printf("Enter Restaurant ID: ");
  fgets(message, sizeof(message), stdin);
  send(client_socket, message, strlen(message), 0);

  restaurant_id = atoi(message);
  // Receive prompts for username and password and send them to the server
  debug_log(3, "Authentication of admin credentials\n");
  int i;
  for (i = 0; i < numAdminCredentials; ++i) {
    if (adminCredentials[i].restaurant_id == restaurant_id) {
      char username[50], password[50];
      retry:

        // Read username
        debug_log(3, "Verfication of admin username\n");
      printf("Enter username: ");
      fgets(username, sizeof(username), stdin);
      username[strcspn(username, "\n")] = '\0'; // Remove newline character
      
      // Send username to server
      send_message(client_socket, username);

      // Read password
      debug_log(3, "Verfiaction of admin password\n");
      printf("Enter password: ");
      fgets(password, sizeof(password), stdin);
      password[strcspn(password, "\n")] = '\0'; // Remove newline character

      // Send password to server
      send_message(client_socket, password);

      if (strcmp(username, adminCredentials[i].username) == 0 && strcmp(password, adminCredentials[i].password) == 0) {
        printf("Login Successful\n");
        debug_log(3, "Login Successful\n");
      } else {
        debug_log(3, "Invalid username and password\n");
        printf("Invalid Username or Password\n");
        printf("Retry: \n");

        goto retry;
      }

      // Perform admin tasks
      int b;
      while (1) {
        debug_log(2, "Admin tasks\n");
        l:
        printf("Admin tasks:\n");
        printf("1. Add item\n");
        printf("2. Remove item\n");
        printf("3. Update item\n");
        printf("4. View order history\n");
        printf("5. Overall Sales\n");
        printf("6. Display Menu\n");
        printf("7. Exit\n");
        printf("\nEnter option: ");

        fgets(message, sizeof(message), stdin);

        // Check if the input is not empty
        if (isdigit(message[0])) {
          send_message(client_socket, message);

          b = atoi(message);

          switch (b) {
          case 1:
            debug_log(3, "Call of add item function\n");
            add_item(client_socket);

            debug_log(3, "Call of Receive menu function\n");
            receive_menu(client_socket);

            break;
          case 2:
            debug_log(3, "Call of remove item function\n");
            remove_item(client_socket);

            break;
          case 3:
            debug_log(3, "Call of update item function\n");
            update_item(client_socket);

            break;
          case 4:
            debug_log(3, "Call of display order function\n");
            display_order_history(client_socket);

            break;
          case 5:
            int time_interval;
            debug_log(3, "Overall Sales display\n");
            // Get the time interval from the user
            printf("Enter the time interval (1 for DAILY, 2 for WEEKLY, 3 for MONTHLY): ");
            scanf("%d", & time_interval);
            send(client_socket, & time_interval, sizeof(time_interval), 0);

            memset(message, 0, sizeof(message));
            recv(client_socket, message, sizeof(message) - 1, 0);

            printf("Server response: %s\n", message);
            break;
          case 6:
            debug_log(3, "Call of receive menu function\n");
            receive_menu(client_socket);

            goto l;
            break;
          case 7:
            debug_log(3, "Exit from admin tasks\n");
            printf("Exiting...\n\n");
            break;
          default:
            debug_log(3, "Invalid choice\n");
            printf("Invalid Choice\n\n");
          }

          if (b == 7) {
            break; // Break the loop if the user chose option 7 to exit
          }
        } else {
           debug_log(2, "Input is not numeric\n");
          // Input is not numeric
          printf("Invalid input. Please enter a numeric choice.\n");
        }

        debug_log(3, "Clear input buffer for admin interface\n");
        clear_input_buffer(); // Clear input buffer
      }
    }
  }
}

void customer_menu(int client_socket) {
  debug_log(2, "Entering to receive menu function\n");
  char buffer[1024];
  int bytes_received;
  debug_log(3, "Variables created for receive menu function\n");
  // Receive menu data from the server

  while (1) {

    // Clear the buffer for the next iteration
    memset(buffer, 0, sizeof(buffer));
    debug_log(3, "Clearing the buffer in receive menu function\n");
    // Receive data from the server
    char ack[] = "ACK";
    send(client_socket, ack, strlen(ack), 0);
    sleep(1);
    bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    debug_log(3, "Receiving data from server for the receive menu to the customer\n");

    if (bytes_received < 0) {
      debug_log(3, "Error receiving data for the receive menu to the customer\n");
      perror("Error receiving data");
      exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {
      debug_log(3, "Server connection closed for the receive menu to the customer\n");
      // Server closed the connection
      break;

    }

    // Check if it's the end of data message
    if (strncmp(buffer, "END", 3) == 0) {

      debug_log(3, "Waiting for end message from server for the receive menu to the customer\n");
      break;

    }

    // Print received menu data
    printf("%s", buffer);

    debug_log(3, "Printing received menu data from server\n");
  }

  printf("\nMenu displayed.\n");
}

void place_order(int client_socket) {
  debug_log(2, "Entering to place order function\n");

  // Send customer details
  char name[100];
  char contact_number[20];
  debug_log(3, "Creating variables for place order function\n");
  debug_log(3, "Sending customer details to serever\n");
  printf("Enter name: ");
  scanf("%s", name);
  // Send item name to the server
  send(client_socket, name, sizeof(name), 0);

  // Read contact number
  printf("Enter contact_number: ");
  scanf("%s", contact_number);
  // Send item name to the server
  send(client_socket, contact_number, sizeof(contact_number), 0);

  // Send order details
  char continue_order = 'y';
  do {

    debug_log(3, "Sending order details to server\n");
    int item_id;
    int quantity;

    printf("Enter item ID: ");
    scanf("%d", & item_id);
    printf("Enter quantity: ");
    scanf("%d", & quantity);

    send(client_socket, & item_id, sizeof(item_id), 0);

    send(client_socket, & quantity, sizeof(quantity), 0);

    printf("Do you want to continue ordering? (y/n): ");
    scanf(" %c", & continue_order);

    send(client_socket, & continue_order, sizeof(continue_order), 0);

  } while (continue_order == 'y');

  // Receive and print the invoice
  char invoice[4096];
  memset(invoice, 0, sizeof(invoice));
  recv(client_socket, invoice, sizeof(invoice)-1, 0);

  debug_log(3, "Receiving invoice details from server\n");
  printf("\t---------------------------------------------\n\n");
  printf("Invoice:\n%s\n", invoice);
  printf("\t---------------------------------------------\n");
  printf("\n\t*********Thank You Visit Again*****************\n");
}

void customer_interface(int client_socket) {
  debug_log(1, "Entering into Customer interface\n");
  printf("\n");
  printf("\t---------------------------------------------\n");
  printf("\t\tWelcome to Customer Interface\t\n");
  printf("\t---------------------------------------------\n");
  printf("\n");

  int restaurant_id;
  char message[1024];
  int c;
  printf("Select Restaurant:\n");
  debug_log(2, "Selecting avaliable restaurants\n");
  display_restaurants(client_socket);

  printf("Enter restaurant choice: ");
  scanf("%d", & restaurant_id);
  getchar(); // Consume the newline character
  send(client_socket, & restaurant_id, sizeof(restaurant_id), 0);

  while (1) {

    debug_log(2, "Display of customer tasks\n");
    l2:
    printf("\nCustomer Options:\n");
    printf("1. View Menu\n");
    printf("2. Place Order\n");
    printf("3. Exit\n");
    printf("Enter choice: ");
    fgets(message, sizeof(message), stdin);

    // Check if the input is numeric
    if (isdigit(message[0])) {
      send_message(client_socket, message);

      c = atoi(message);

      switch (c) {
      case 1:
        debug_log(3, "Call of customer menu function\n");
        customer_menu(client_socket);

        goto l2;
        break;
      case 2:
        debug_log(3, "Call of place order function\n");
        place_order(client_socket);

        break;
      case 3:
        debug_log(3, "Exit from the custome tasks\n");
        printf("Exiting...\n");

        break;
      default:
        debug_log(3, "Invalid choice\n");
        printf("Invalid choice\n");
      }
      if (c == 3) {
        break; // Break the loop if the user chose option 7 to exit
      }
    } else {
      // Input is not numeric
      printf("Invalid input. Please enter a numeric choice.\n");
    }
    debug_log(3, "Clear of input buffer for customer interface\n");
    clear_input_buffer(); // Clear input buffer

  }
}

int main(int argc, char * argv[]) {
  setDebugLevel(argc, argv); // Set debug level based on command-line arguments

  debug_log(1, "Enter into main function\n");
  int client_socket;
  struct sockaddr_in server_addr;
  char message[2048];
  char buffer[2048];
  debug_log(2, "Variables created for main function execution\n");
  // Create client socket
  client_socket = socket(AF_INET, SOCK_STREAM, 0);

  debug_log(3, "Creation of client socket connection\n");
  if (client_socket == -1) {
    debug_log(3, "Socket creation failed for main function\n");
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }
  debug_log(3, "Initialize server address structure\n");
  // Initialize server address structure
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  server_addr.sin_port = htons(PORT);

  debug_log(3, "Connect to the server for the main function\n");
  // Connect to the server
  if (connect(client_socket, (struct sockaddr * ) & server_addr, sizeof(server_addr)) == -1) {

    debug_log(3, "Connection failed for the main function\n");
    perror("Connection failed");
    exit(EXIT_FAILURE);
  }

  // Send user type to the server
  debug_log(2, "Send user type to the server\n");
  s1:

  printf("Select User Type:\n");
  printf("1. Admin\n");
  printf("2. Customer\n");
  printf("3. Exit\n");
  printf("Enter choice: ");
  fgets(message, sizeof(message), stdin);
  if (isdigit(message[0])) {
    send_message(client_socket, message);

    int user_type = atoi(message);

    switch (user_type) {
    case 1:
      debug_log(3, "Call of admin interface function\n");
      admin_interface(client_socket);

      goto s1;
      break;
    case 2:
      debug_log(3, "Call of customer interface function\n");
      customer_interface(client_socket);

      goto s1;
      break;
    case 3:
      debug_log(3, "Exit from user interface\n");
      printf("Exiting...\n");
      break;
    default:
      debug_log(3, "Invalid choice\n");
      printf("Invalid Choice\n");
      goto s1;
    }
  } else {
    // Input is not numeric
    debug_log(2, "Input is not numeric\n");
    printf("Invalid input. Please enter a numeric choice.\n");

    goto s1;
  }
  debug_log(3, "Close the client socket connection\n");
  // Close the client socket
  close(client_socket);

  return 0;

}
