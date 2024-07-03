package auth

import (
	"os"

	"github.com/gorilla/sessions"
	"github.com/markbates/goth"
	"github.com/markbates/goth/gothic"
	"github.com/markbates/goth/providers/google"
)

type cfg struct {
	red string
}

func NewAuth() {
	redimpl := cfg{
		red: "http://localhost:8080/auth/callback?provider=google",
	}
	// Set up session store
	key := os.Getenv("SESSION_SECRET")
	maxAge := 86400 * 30 // 30 days
	isProd := false      // Set to true when serving over https
	store := sessions.NewCookieStore([]byte(key))
	store.MaxAge(maxAge)
	store.Options.Path = "/"
	store.Options.HttpOnly = true // HttpOnly should be enabled
	store.Options.Secure = isProd

	gothic.Store = store
	goth.UseProviders(google.New(
		os.Getenv("GOOGLE_CLIENT_ID"),
		os.Getenv("GOOGLE_CLIENT_SECRET"),
		redimpl.red))

}
