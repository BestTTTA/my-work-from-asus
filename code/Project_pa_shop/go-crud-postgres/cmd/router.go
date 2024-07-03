package main

import (
	// "net/http"

	"github.com/BestTTTA/go-crud-postgres/app/handlers"
	"github.com/BestTTTA/go-crud-postgres/app/products"
	"github.com/gin-gonic/gin"
	"gorm.io/gorm"
)


func ProductRouter(router *gin.Engine, db *gorm.DB) {

	productValidation := products.NewAdaptor(db)
	productCore := products.NewCore(db)
	productUsecase := products.NewUsecase(productCore, productValidation)
	productHandler := products.NewHandler(productUsecase)

	router.POST("/product", productHandler.NewProduct)
	router.GET("/products", productHandler.ListProducts)
	router.GET("/product/:id", productHandler.GetProductByID)
	router.DELETE("/product/:id", productHandler.DeleteProduct)
	router.PUT("/product", productHandler.UpdateProduct)
}



func Register(router *gin.Engine) {
	loginHandler := handlers.NewLoginHandler()
	logoutHandler := handlers.NewLogoutHandler()

	// router.LoadHTMLGlob("templates/*")

	// // Define routes
	// router.GET("/", func(c *gin.Context) {
	// 	providers := handlers.GetProviders()
	// 	c.HTML(http.StatusOK, "index.html", gin.H{"providers": providers})
	// })

	router.GET("/auth/callback", loginHandler.AuthCallback)
	router.GET("/logout/:provider", logoutHandler.Logout)
	router.GET("/auth", loginHandler.Auth)
}

