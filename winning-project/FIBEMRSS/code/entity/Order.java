package com.ischoolbar.programmer.entity;

import java.util.Date;
import java.util.List;

import org.springframework.stereotype.Component;

/**
 * 订单实体类
 * @author lenovo
 *
 */
@Component
public class Order {
	private Long id;
	private Long accountId;//客户id
	private float money;//订单金额
	private int productNum;//商品数量
	private int status = 0;//订单状态,0：待发货，1：已发货，2：已完成
	private String recieveName;//收件人
	private String phone;//手机号码
	private String address;//地址
	private Date createTime;//下单时间
	private List<OrderItem> orderItems;//订单子项集合
	private Account account;//订单所属客户
	public Long getId() {
		return id;
	}
	public void setId(Long id) {
		this.id = id;
	}
	public Long getAccountId() {
		return accountId;
	}
	public void setAccountId(Long accountId) {
		this.accountId = accountId;
	}
	public float getMoney() {
		return money;
	}
	public void setMoney(float money) {
		this.money = money;
	}
	public int getProductNum() {
		return productNum;
	}
	public void setProductNum(int productNum) {
		this.productNum = productNum;
	}
	public int getStatus() {
		return status;
	}
	public void setStatus(int status) {
		this.status = status;
	}
	public String getRecieveName() {
		return recieveName;
	}
	public void setRecieveName(String recieveName) {
		this.recieveName = recieveName;
	}
	public String getPhone() {
		return phone;
	}
	public void setPhone(String phone) {
		this.phone = phone;
	}
	public String getAddress() {
		return address;
	}
	public void setAddress(String address) {
		this.address = address;
	}
	public Date getCreateTime() {
		return createTime;
	}
	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}
	public Account getAccount() {
		return account;
	}
	public void setAccount(Account account) {
		this.account = account;
	}
	public List<OrderItem> getOrderItems() {
		return orderItems;
	}
	public void setOrderItems(List<OrderItem> orderItems) {
		this.orderItems = orderItems;
	}
	
	
	
	
}
