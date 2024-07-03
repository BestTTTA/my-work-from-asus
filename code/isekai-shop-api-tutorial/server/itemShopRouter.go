package server

import (
	_itemShopController "github.com/BestTTTA/isekai-shop-api-tutorial/pkg/itemShop/controller"
	_itemShopRepository "github.com/BestTTTA/isekai-shop-api-tutorial/pkg/itemShop/repository"
	_itemShopService "github.com/BestTTTA/isekai-shop-api-tutorial/pkg/itemShop/service"
)

func (s *echoServer) initItemShopRouter() {
	router := s.app.Group("/v1/item-shop")

	itemShopRepository := _itemShopRepository.NewItemShopRepository(s.db, s.app.Logger)
	itemShopService := _itemShopService.NewItemShopServiceImpl(itemShopRepository)
	itemShopController := _itemShopController.NewItemShopControllerImpl(itemShopService)

	router.GET("", itemShopController.Listing)
}
