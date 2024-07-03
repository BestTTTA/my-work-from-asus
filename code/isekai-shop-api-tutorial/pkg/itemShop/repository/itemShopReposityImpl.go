package repository

import (
	"github.com/BestTTTA/isekai-shop-api-tutorial/entities"
	"github.com/labstack/echo/v4"
	"gorm.io/gorm"

	_itemShopModel "github.com/BestTTTA/isekai-shop-api-tutorial/pkg/itemShop/model"
)

type itemShopRepositoryImpl struct {
	db     *gorm.DB
	logger echo.Logger
}

func NewItemShopRepository(db *gorm.DB, logger echo.Logger) ItemShopRepository {
	return &itemShopRepositoryImpl{db, logger}
}

func (r *itemShopRepositoryImpl) Listing(itemFilter *_itemShopModel.ItemFilter) ([]*entities.Item, error) {
	itemList := make([]*entities.Item, 0)
	query := r.db.Model(&entities.Item{})

	if itemFilter.Name != "" {
		query = query.Where("name ilike ?", "%"+itemFilter.Name+"%")
	}
	if itemFilter.Description != "" {
		query = query.Where("description ilike ?", "%"+itemFilter.Description+"%")
	}

	if err := query.Find(&itemList).Error; err != nil {
		r.logger.Errorf("Failded to list items: %s", err.Error())
		return nil, err
	}
	return itemList, nil
}
