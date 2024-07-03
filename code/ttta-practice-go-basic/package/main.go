package main

import (
	// "fmt"
	// "practice-go/calculator"
	"github.com/labstack/echo/v4"
	"net/http"

)

func main() {
	// fmt.Print(calculator.Add(1, 2))

	e := echo.New()
	e.GET("/", func(c echo.Context) error {
		return c.String(http.StatusOK, "Hello, World!")
	})
	e.Logger.Fatal(e.Start(":1323"))
}