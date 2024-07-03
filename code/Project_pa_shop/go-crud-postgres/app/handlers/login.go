package handlers

import (
	// "fmt"
	"fmt"
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/markbates/goth/gothic"
)

// LoginHandler handles the login process.
type LoginHandler struct{}

func NewLoginHandler() *LoginHandler {
	return &LoginHandler{}
}

// Auth initiates the authentication process.
func (h *LoginHandler) Auth(c *gin.Context) {
	gothic.BeginAuthHandler(c.Writer, c.Request)
}

// AuthCallback handles the callback from the provider.
func (h *LoginHandler) AuthCallback(c *gin.Context) {
	user, err := gothic.CompleteUserAuth(c.Writer, c.Request)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}
	fmt.Println(user)
	c.Redirect(http.StatusFound, "http://localhost:3000/user")
	// c.JSON(http.StatusOK, user)
}

func GetProviders() map[string]string {
	providers := make(map[string]string)
	providers["google"] = "Google"
	return providers
}
