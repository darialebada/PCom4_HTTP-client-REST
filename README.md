# Web Client. Communication with a REST API

## Goal

The goal of this project is to implement a **web client in C** that interacts with a **REST API**.

---

## Request Implementation (`request.c`)

The `request.c` file was completed according to the comments and specifications, respecting the HTTP request format.

For **GET requests**, the following elements are included:

- HTTP method name
- request path
- protocol version (`HTTP/1.1`)
- host (`34.254.242.81`)
- JWT token (if available)
- cookies (if available)

For **POST requests**, in addition to the elements above, the following **Entity Headers** are added:

- `Content-Type`
- `Content-Length`

The request body containing the actual data is added at the end.

For **DELETE requests**, since no additional data is attached, the request structure is similar to a GET request.  
However, cookies are not required because deleting a book requires only the **JWT token**.

---

## Client Implementation (`client.c`)

Two variables are used during program execution:

- `logged_in` – indicates whether a user is currently logged in
- `has_token` – indicates whether the current user has access to the library

The client reads commands from **stdin** and sends the necessary requests to the server.

---

### REGISTER

A new user can register **only if no user is currently logged in**.

After validating the input data (ensuring that no spaces are present), the client sends a **POST request** containing the username and password.

The server response is processed to determine whether the chosen username is available.

After displaying the result message, the memory is freed and the connection with the server is closed until another valid action is requested.

Since most commands follow a similar structure to `REGISTER`, the following explanations are presented more concisely.

---

### LOGIN

Similar to `REGISTER`, but if the credentials are valid, the server returns a response containing **cookies**, which are parsed and stored for later use.

The client also marks that a user is now **logged in**.

---

### ENTER_LIBRARY

If a user is logged in, the server returns a **JWT token**, which proves that the current user has access to the library.

The token is stored for future requests.

---

### GET_BOOKS

If the logged-in user has access to the library, the client retrieves and displays all available books, showing:

- book ID
- title

---

### GET_BOOK

Similar to the previous command, but displays detailed information about a single book based on its ID:

- title
- author
- genre
- publisher
- number of pages

The command is executed only if the provided ID is valid.

---

### ADD_BOOK

The user can add a new book to the library.

Input validation rules:

- **Page count** must not contain letters or special characters.
- Other fields must not contain only digits (they are strings), but may contain both letters and numbers.  
  For example:
  - `"Volum 1"` → valid
  - `"1"` → invalid

---

### DELETE_BOOK

The client sends a **book ID** to the server.  
If the ID is valid, the corresponding book is removed from the library.

---

### LOGOUT

Logs out the current user and frees the memory allocated for:

- cookies
- JWT token (if present)

---

### EXIT

Releases any remaining allocated memory and terminates the program.

---

### Invalid Command

If an invalid command is entered, the user is notified about the valid commands that can be used.

---

## JSON Parsing

The **Parson library** was used for JSON parsing because it was the easiest option.

The library documentation and README were very helpful for implementing the parsing logic.

To convert messages sent to the server into JSON format:

- `json_value_init_object()` is used to initialize a JSON object
- fields are added according to the required structure

To parse JSON responses received from the server:

- `json_parse_string()` is used to parse the JSON string
- values are extracted using the appropriate **get functions** provided by the library.
