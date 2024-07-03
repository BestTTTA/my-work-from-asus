package products

import (
	"context"
	"errors"
	"net/http"
	"strconv"

	"github.com/gin-gonic/gin"
	// "gorm.io/gorm"
)

type usecase interface {
	NewProduct(ctx context.Context, product Product) error
	ListProducts(ctx context.Context) (*[]ProductEntity, error)
	DeleteProduct(ctx context.Context, id uint) error
	UpdateProduct(ctx context.Context, product Product) error
	GetProductByID(ctx context.Context, id uint) (*Product, error)
}

type Handler struct {
	usecase usecase
}

func NewHandler(u usecase) *Handler {
	return &Handler{usecase: u}
}

func (h *Handler) NewProduct(c *gin.Context) {
	var req NewProductRequest

	if err := c.Bind(&req); err != nil {
		c.JSON(http.StatusBadRequest, map[string]string{
			"error": err.Error(),
		})
		return
	}

	product := Product{
		Name:  req.Name,
		Price: req.Price,
		ImageUrl: req.ImageUrl,
		Count: req.Count,
		Type: req.Type,
	}

	if err := h.usecase.NewProduct(context.Background(), product); err != nil {
		if errors.Is(err, ErrProductNameExists) {
			c.JSON(http.StatusConflict, gin.H{"error": "product name already exists"})
			return
		}
		c.AbortWithStatus(http.StatusInternalServerError)
		return
	}

	c.JSON(http.StatusOK, gin.H{"message": "Product created successfully"})
}


// NewRequest for get List of Products
func (h *Handler) ListProducts(c *gin.Context) {
	products, err := h.usecase.ListProducts(context.Background())
	if err != nil {
		c.AbortWithStatus(500)
		return
	}

	c.JSON(http.StatusOK, gin.H{"products": products})
}

func (h *Handler) DeleteProduct(c *gin.Context) {
	idParam := c.Param("id")
	idInt, err := strconv.Atoi(idParam)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "invalid product ID"})
		return
	}
	id := uint(idInt) 

	err = h.usecase.DeleteProduct(context.Background(), id)
	if err != nil {
		c.AbortWithStatus(http.StatusInternalServerError)
		return
	}

	c.JSON(http.StatusOK, gin.H{"message": "Product deleted successfully"})
}


func (h *Handler) GetProductByID(c *gin.Context) {
	idParam := c.Param("id")
	idInt, err := strconv.Atoi(idParam)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "invalid product ID"})
		return
	}
	id := uint(idInt)

	product, err := h.usecase.GetProductByID(context.Background(), id)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "could not retrieve product"})
		return
	}

	c.JSON(http.StatusOK, gin.H{"product": product})
}


func (h *Handler) UpdateProduct(c *gin.Context) {
	var req UpdateProductRequest

	if err := c.Bind(&req); err != nil {
		c.JSON(http.StatusBadRequest, map[string]string{
			"error": err.Error(),
		})
		return
	}

	// Fetch the existing product
	existingProduct, err := h.usecase.GetProductByID(context.Background(), req.ID)
	if err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "product not found"})
		return
	}

	// Update only the provided fields
	if req.Name != "" {
		existingProduct.Name = req.Name
	}
	if req.Price != 0 {
		existingProduct.Price = req.Price
	}
	if req.Count != 0 {
		existingProduct.Count = req.Count
	}
	if req.ImageUrl != "" {
		existingProduct.ImageUrl = req.ImageUrl
	}

	if err := h.usecase.UpdateProduct(context.Background(), *existingProduct); err != nil {
		if errors.Is(err, ErrProductNameExists) {
			c.JSON(http.StatusConflict, gin.H{"error": "product name already exists"})
			return
		}
		c.AbortWithStatus(http.StatusInternalServerError)
		return
	}

	c.JSON(http.StatusOK, gin.H{"message": "Product updated successfully"})
}