package com.ischoolbar.programmer.entity;

import org.springframework.stereotype.Component;

/**
 * 食物信息实体类
 * @author lenovo
 *
 */
@Component
public class Food {
	private Long id;
	private Long categoryId;//菜品分类id
	private String name;//菜品名称
	private float price;
	private Integer sels = 0;//销量
	private String imageUrl;//图片地址
	private String des;//菜品描述信息
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
	public Long getCategoryId() {
		return categoryId;
	}
	public void setCategoryId(Long categoryId) {
		this.categoryId = categoryId;
	}
	public float getPrice() {
		return price;
	}
	public void setPrice(float price) {
		this.price = price;
	}
	public Integer getSels() {
		return sels;
	}
	public void setSels(Integer sels) {
		this.sels = sels;
	}
	public String getImageUrl() {
		return imageUrl;
	}
	public void setImageUrl(String imageUrl) {
		this.imageUrl = imageUrl;
	}
	public String getDes() {
		return des;
	}
	public void setDes(String desc) {
		this.des = desc;
	}
}
