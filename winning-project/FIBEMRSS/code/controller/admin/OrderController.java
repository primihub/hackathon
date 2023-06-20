package com.ischoolbar.programmer.controller.admin;

import java.util.HashMap;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.servlet.ModelAndView;

import com.ischoolbar.programmer.entity.Order;
import com.ischoolbar.programmer.page.amdin.Page;
import com.ischoolbar.programmer.service.OrderService;
import com.mysql.jdbc.StringUtils;

/**
 * 订单信息后台管理控制器
 * @author llq
 *
 */
@RequestMapping("/admin/order")
@Controller
public class OrderController {
	
	@Autowired
	private OrderService orderService;
	
	/**
	 * 订单信息列表页面
	 * @param model
	 * @return
	 */
	@RequestMapping(value="/list",method=RequestMethod.GET)
	public ModelAndView list(ModelAndView model){
		model.setViewName("order/list");
		return model;
	}
	
	/**
	 * 模糊分页查询订单信息
	 * @return
	 */
	@RequestMapping(value="/list",method=RequestMethod.POST)
	@ResponseBody
	public Map<String, Object> getList(
			@RequestParam(name="name",defaultValue="") String name,
			@RequestParam(name="status",required=false) Integer status,
			Page page
			){
		Map<String, Object> ret = new HashMap<String, Object>();
		Map<String, Object> queryMap = new HashMap<String, Object>();
		queryMap.put("name", name);
		if(status != null && status != -1){
			queryMap.put("status", status);
		}
		queryMap.put("offset", page.getOffset());
		queryMap.put("pageSize", page.getRows());
		ret.put("rows", orderService.findList(queryMap));
		ret.put("total", orderService.getTotal(queryMap));
		return ret;
	}
	
	
	
	/**
	 * 订单信息编辑
	 * @param order
	 * @return
	 */
	@RequestMapping(value="/edit",method=RequestMethod.POST)
	@ResponseBody
	public Map<String, String> edit(Order order){
		Map<String, String> ret = new HashMap<String, String>();
		if(order == null){
			ret.put("type", "error");
			ret.put("msg", "请填写正确的订单信息!");
			return ret;
		}
		if(StringUtils.isNullOrEmpty(order.getRecieveName())){
			ret.put("type", "error");
			ret.put("msg", "订单联系人不能为空!");
			return ret;
		}
		if(StringUtils.isNullOrEmpty(order.getPhone())){
			ret.put("type", "error");
			ret.put("msg", "订单联系方式不能为空!");
			return ret;
		}
		if(StringUtils.isNullOrEmpty(order.getAddress())){
			ret.put("type", "error");
			ret.put("msg", "订单联系地址不能为空!");
			return ret;
		}
		if(orderService.edit(order) <= 0){
			ret.put("type", "error");
			ret.put("msg", "订单信息修改失败，请联系管理员!");
			return ret;
		}
		ret.put("type", "success");
		ret.put("msg", "修改成功！");
		return ret;
	}
	
	/**
	 * 订单信息删除
	 * @param id
	 * @return
	 */
	@RequestMapping(value="/delete",method=RequestMethod.POST)
	@ResponseBody
	public Map<String, String> delete(Long id){
		Map<String, String> ret = new HashMap<String, String>();
		if(id == null){
			ret.put("type", "error");
			ret.put("msg", "请选择要删除的分类信息!");
			return ret;
		}
		try {
			if(orderService.delete(id) <= 0){
				ret.put("type", "error");
				ret.put("msg", "订单信息删除失败，请联系管理员!");
				return ret;
			}
		} catch (Exception e) {
			// TODO: handle exception
			ret.put("type", "error");
			ret.put("msg", "该订单信息下存在子订单信息，请先删除相关菜品信息!");
			return ret;
		}
		ret.put("type", "success");
		ret.put("msg", "删除成功！");
		return ret;
	}
}