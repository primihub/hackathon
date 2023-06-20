package com.ischoolbar.programmer.entity;

import java.util.List;

import org.springframework.stereotype.Component;

/**
 * 菜品分类信息实体
 * @author lenovo
 *
 */

@Component
public class FoodCategory {

	private Long id;
	private String name;//菜品分类名称
	private List<Food> foodList;//该分类下的所有菜品信息
	public Long getId() {
		return id;
	}
	public void setId(Long id) {
		this.id = id;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public List<Food> getFoodList() {
		return foodList;
	}
	public void setFoodList(List<Food> foodList) {
		this.foodList = foodList;
	}
	
	
}
