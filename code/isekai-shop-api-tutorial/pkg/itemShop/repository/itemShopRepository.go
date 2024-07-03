package repository

import (
	"github.com/BestTTTA/isekai-shop-api-tutorial/entities"
	_itemShopModel "github.com/BestTTTA/isekai-shop-api-tutorial/pkg/itemShop/model"
)

type ItemShopRepository interface {
	Listing(itemFilter *_itemShopModel.ItemFilter) ([]*entities.Item, error)
}
