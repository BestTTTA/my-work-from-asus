package config

import (
	"log/slog"
	"sync"
	"time"

	"github.com/caarlos0/env/v10"
)

type config struct {
	Port             string        `env:"PORT" envDefault:"8080"`
	AllowedOrigins   []string      `env:"ALLOWED_ORIGINS"`
	AllowedMethods   []string      `env:"ALLOWED_METHODS" envDefault:"GET,POST,PUT,PATCH,DELETE,HEAD,OPTIONS"`
	AllowedHeaders   []string      `env:"ALLOWED_HEADERS" envDefault:"X-Requested-With,Authorization,Origin,Content-Length,Content-Type"`
	AllowCredentials bool          `env:"ALLOW_CREDENTIALS" envDefault:"true"`
	CorsMaxAge       time.Duration `env:"CORS_MAX_AGE" envDefault:"5s"`
}

var Val config
var once sync.Once

func init() {
	once.Do(
		func() {
			cfg := config{}
			if err := env.Parse(&cfg); err != nil {
				slog.Warn(err.Error())
			}

			Val = cfg
		},
	)
}
