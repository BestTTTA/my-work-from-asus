package main

import (
	"context"
	"fmt"
	"log"
	"net/http"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/BestTTTA/go-crud-postgres/auth"
	"github.com/BestTTTA/go-crud-postgres/config"
	"github.com/BestTTTA/go-crud-postgres/database"
	"github.com/BestTTTA/go-crud-postgres/initializers"
	"github.com/gin-gonic/gin"
)

func init() {
	initializers.LoadEnvVariables()
}

func main() {

	auth.NewAuth()
	db := database.NewGormDBWithDefault()

	r := gin.Default()

	r.Use(CORSMiddleware())
	
	ProductRouter(r, db)
	Register(r)

	srv := http.Server{
		Addr:              ":" + config.Val.Port,
		Handler:           r,
		ReadHeaderTimeout: 5 * time.Second,
	}

	idleConnsClosed := make(chan struct{})

	go func() {
		sigint := make(chan os.Signal, 1)
		signal.Notify(sigint, syscall.SIGINT, syscall.SIGTERM)
		<-sigint

		d := time.Duration(5 * time.Second)
		fmt.Printf("shutting down int %s ...", d)
		ctx, cancel := context.WithTimeout(context.Background(), d)
		defer cancel()
		if err := srv.Shutdown(ctx); err != nil {
			log.Fatal(err)
		}
		close(idleConnsClosed)
	}()

	fmt.Println(":" + config.Val.Port + " is serve")

	if err := srv.ListenAndServe(); err != http.ErrServerClosed {
		log.Fatal(err)
		return
	}

	<-idleConnsClosed
	fmt.Println("gracefully")
}
