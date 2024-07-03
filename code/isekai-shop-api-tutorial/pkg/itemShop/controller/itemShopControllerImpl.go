package controller

import (
	"net/http"

	"github.com/BestTTTA/isekai-shop-api-tutorial/pkg/custom"
	_itemShopModel "github.com/BestTTTA/isekai-shop-api-tutorial/pkg/itemShop/model"
	_itemShopService "github.com/BestTTTA/isekai-shop-api-tutorial/pkg/itemShop/service"
	"github.com/labstack/echo/v4"
	// _itemShopException "github.com/BestTTTA/isekai-shop-api-tutorial/pkg/itemShop/exception"
)

type itemShopControllerImpl struct {
	itemShopService _itemShopService.ItemShopService
}

func NewItemShopControllerImpl(itemShopService _itemShopService.ItemShopService) ItemShopController {
	return &itemShopControllerImpl{itemShopService}
}

func (c *itemShopControllerImpl) Listing(pctx echo.Context) error {
	ItemFilter := new(_itemShopModel.ItemFilter)

	customEchoRequest := custom.NewCustomEchoRequest(pctx)
	
	if err := customEchoRequest.Bind(ItemFilter); err != nil {
		return custom.Error(pctx, http.StatusBadRequest, err.Error())
	}

	itemModelList, err := c.itemShopService.Listing(ItemFilter)
	if err != nil {
		return custom.Error(pctx, http.StatusInternalServerError, err.Error())
	}

	return pctx.JSON(http.StatusOK, itemModelList)
	// return custom.Error(pctx, http.StatusInternalServerError, (&_itemShopException.ItemListing{}).Error())
}
