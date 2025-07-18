#include <cstddef>

// String Manipulation

char *strcpy(char *dest, const char *src) {
	char *original_dest = dest;
	while (*src) {
		*dest++ = *src++;
	}
	*dest = '\0'; // Null-terminate the destination string
	return original_dest;
}


// String Examination

size_t strlen(const char *str) {
	const char *end = str;
	while (*end != '\0')
		++end;
	return end - str;
}

int strcmp(const char *lhs, const char *rhs) {
	while (*lhs && (*lhs == *rhs)) {
		lhs++;
		rhs++;
	}
	return *(unsigned char *) lhs - *(unsigned char *) rhs;
}

int strncmp(const char *lhs, const char *rhs, size_t num) {
	while (num && *lhs && (*lhs == *rhs)) {
		lhs++;
		rhs++;
		num--;
	}
	if (num == 0) {
		return 0; // Strings are equal up to num characters
	}
	return *(unsigned char *) lhs - *(unsigned char *) rhs;
}

char *strchr(const char *str, int ch) {
	while (*str) {
		if (*str == (char) ch) {
			return (char *) str; // Return pointer to the first occurrence
		}
		str++;
	}
	return NULL; // Character not found
}

char *strrchr(const char *str, int ch) {
	const char *last = NULL;
	while (*str) {
		if (*str == (char) ch) {
			last = str; // Update last occurrence
		}
		str++;
	}
	return (char *) last; // Return pointer to the last occurrence or NULL if not found
}


// Character Array Manipulation

void *memset(void *dest, int ch, size_t count) {
	unsigned char *dst = (unsigned char *) dest;
	for (size_t i = 0; i < count; i++) {
		dst[i] = (unsigned char) ch;
	}
	return dest;
}

void *memcpy(void *dest, const void *src, size_t count) {
	unsigned char *d = (unsigned char *) dest;
	const unsigned char *s = (const unsigned char *) src;
	for (size_t i = 0; i < count; i++) {
		d[i] = s[i];
	}
	return dest;
}

char *strtok(char *str, const char *delim) {
	static char *last = NULL;
	if (str == NULL) {
		str = last;
	}
	if (str == NULL) {
		return NULL; // No more tokens
	}

	// Skip leading delimiters
	while (*str && strchr(delim, *str)) {
		str++;
	}

	if (*str == '\0') {
		last = NULL; // No more tokens
		return NULL;
	}

	char *token_start = str;

	// Find the end of the token
	while (*str && !strchr(delim, *str)) {
		str++;
	}

	if (*str != '\0') {
		*str = '\0'; // Null-terminate the token
		last = str + 1; // Move to the next character after the delimiter
	} else {
		last = NULL; // No more tokens
	}

	return token_start;
}
