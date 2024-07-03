package products

import (
	"context"

	"gorm.io/gorm"
)

type Core struct {
	db *gorm.DB
}

func NewCore(db *gorm.DB) *Core {
	return &Core{db: db}
}

func (c *Core) CreateProduct(ctx context.Context, product ProductEntity) (*uint, error) {
	if err := c.db.Create(&product); err.Error != nil {
		return nil, err.Error
	}
	return &product.ID, nil
}

func (c *Core) GetProductByID(ctx context.Context, id uint) (*ProductEntity, error) {
	var product ProductEntity
	if err := c.db.First(&product, id).Error; err != nil {
		return nil, err
	}
	return &product, nil
}

func (c *Core) ListProducts(ctx context.Context) (*[]ProductEntity, error) {
	var products []ProductEntity
	if err := c.db.Find(&products).Error; err != nil {
		return nil, err
	}
	return &products, nil
}


func (c *Core) UpdateProduct(ctx context.Context, product ProductEntity) error {
	return c.db.Model(ProductEntity{}).Where("id = ?", product.ID).Updates(product).Error
}

func (c *Core) DeleteProduct(ctx context.Context, id uint) error {
	return c.db.Delete(&ProductEntity{}, id).Error
}
