package com.ischoolbar.programmer.entity;

import java.util.Date;
import java.util.List;

import org.springframework.stereotype.Component;

/**
 * 订单子项目实体类
 * @author lenovo
 *
 */
@Component
public class OrderItem {
	private Long id;
	private Long orderId;//所属订单id
	private Long foodId;//商品id
	private String foodName;//商品名称
	private String foodImage;//商品图片
	private float price;//商品价格
	private int foodNum;//商品数量
	private float money;//价格
	public Long getId() {
		return id;
	}
	public void setId(Long id) {
		this.id = id;
	}
	public Long getOrderId() {
		return orderId;
	}
	public void setOrderId(Long orderId) {
		this.orderId = orderId;
	}
	public Long getFoodId() {
		return foodId;
	}
	public void setFoodId(Long foodId) {
		this.foodId = foodId;
	}
	public String getFoodName() {
		return foodName;
	}
	public void setFoodName(String foodName) {
		this.foodName = foodName;
	}
	public String getFoodImage() {
		return foodImage;
	}
	public void setFoodImage(String foodImage) {
		this.foodImage = foodImage;
	}
	public float getPrice() {
		return price;
	}
	public void setPrice(float price) {
		this.price = price;
	}
	public int getFoodNum() {
		return foodNum;
	}
	public void setFoodNum(int foodNum) {
		this.foodNum = foodNum;
	}
	public float getMoney() {
		return money;
	}
	public void setMoney(float money) {
		this.money = money;
	}
	
	
	
	
}
