package soloscuro

import (
    "github.com/google/uuid"
)

type PlayerState struct {
    Username, Password string
    UUID uuid.UUID
}

