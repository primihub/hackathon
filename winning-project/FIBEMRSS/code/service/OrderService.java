package com.ischoolbar.programmer.service;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Service;

import com.ischoolbar.programmer.entity.Order;
import com.ischoolbar.programmer.entity.OrderItem;


/**
 * 订单service
 * @author lenovo
 *
 */
@Service
public interface OrderService {
	public int add(Order order);
	public int addItem(OrderItem orderItem);
	public int edit(Order Order);
	public List<Order> findList(Map<String, Object> queryMap);
	public Integer getTotal(Map<String, Object> queryMap);
	public int delete(Long id);
	public int elter(Long id);
}
