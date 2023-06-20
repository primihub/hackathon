package com.ischoolbar.programmer.controller.home;

import java.util.HashMap;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.servlet.ModelAndView;

import com.ischoolbar.programmer.entity.Account;
import com.ischoolbar.programmer.entity.Order;
import com.ischoolbar.programmer.service.AccountService;
import com.ischoolbar.programmer.service.OrderService;

/**
 * 前台用户控制器
 * @author lenovo
 *
 */
@RequestMapping("/home/user")
@Controller
public class HomeAccountController {
	
	@Autowired
	private OrderService orderService;
	
	@Autowired
	private AccountService accountService;
	
	
	
	/**
	 * 用户中心首页
	 * @param model
	 * @return
	 */
	@RequestMapping(value="/index",method=RequestMethod.GET)
	public String index(HttpServletRequest request,Model model){
		Object attribute = request.getSession().getAttribute("account");
		if(attribute == null){
			return "redirect:login";
		}
		Account loginedAccount = (Account)attribute;
		Map<String, Object> queryMap = new HashMap<String, Object>();
		queryMap.put("accountId", loginedAccount.getId());
		queryMap.put("offset", 0);
		queryMap.put("pageSize", 999);
		model.addAttribute("orderList", orderService.findList(queryMap));
		return "home/user/index";
	}
	/**
	 * 用户登录页面
	 * @param model
	 * @return
	 */
	@RequestMapping(value="/login",method=RequestMethod.GET)
	public ModelAndView login(ModelAndView model){
		model.setViewName("home/user/login");
		return model;
	}
	
	/**
	 * 用户注册页面
	 * @param model
	 * @return
	 */
	@RequestMapping(value="/registe",method=RequestMethod.GET)
	public ModelAndView registe(ModelAndView model){
		model.setViewName("home/user/registe");
		return model;
	}
	
	/**
	 * 获取用户基本信息
	 * @return
	 */
	@RequestMapping(value="/get_user_address",method=RequestMethod.POST)
	@ResponseBody
	public Map<String,String> getFoodList(HttpServletRequest request){
		Map<String, String> ret = new HashMap<String, String>();
		Object attribute = request.getSession().getAttribute("account");
		if(attribute == null){
			ret.put("type", "error");
			ret.put("msg", "请先登录");
			return ret;
		}
		Account account = (Account) attribute;
		ret.put("type", "success");
		ret.put("name", account.getRealName());
		ret.put("address", account.getAddress());
		ret.put("phone", account.getPhone());
		return ret;
	}
	
	
	/**
	 * 用户注册
	 * @param account
	 * @return
	 */
	@RequestMapping(value="/registe",method=RequestMethod.POST)
	@ResponseBody
	public Map<String,String> registe(Account account){
		Map<String, String> ret = new HashMap<String, String>();
		if(account == null){
			ret.put("type", "error");
			ret.put("msg", "请填写正确的信息！");
			return ret;
		}
		if(StringUtils.isEmpty(account.getName())){
			ret.put("type", "error");
			ret.put("msg", "用户名不能为空！");
			return ret;
		}
		if(StringUtils.isEmpty(account.getPassword())){
			ret.put("type", "error");
			ret.put("msg", "密码不能为空！");
			return ret;
		}
		if(isExist(account.getName(), 0l)){
			ret.put("type", "error");
			ret.put("msg", "该用户名已经存在！");
			return ret;
		}
		if(accountService.add(account) <= 0){
			ret.put("type", "error");
			ret.put("msg", "注册失败，请联系管理员！");
			return ret;
		}
		ret.put("type", "success");
		ret.put("msg", "注册成功!");
		return ret;
	}
	
	/**
	 * 用户登录
	 * @param account
	 * @param request
	 * @return
	 */
	@RequestMapping(value="/login",method=RequestMethod.POST)
	@ResponseBody
	public Map<String,String> login(Account account,HttpServletRequest request){
		Map<String, String> ret = new HashMap<String, String>();
		if(account == null){
			ret.put("type", "error");
			ret.put("msg", "请填写正确的信息！");
			return ret;
		}
		if(StringUtils.isEmpty(account.getName())){
			ret.put("type", "error");
			ret.put("msg", "用户名不能为空！");
			return ret;
		}
		if(StringUtils.isEmpty(account.getPassword())){
			ret.put("type", "error");
			ret.put("msg", "密码不能为空！");
			return ret;
		}
		Account findByName = accountService.findByName(account.getName());
		if(findByName == null){
			ret.put("type", "error");
			ret.put("msg", "用户名不存在！");
			return ret;
		}
		if(!findByName.getPassword().equals(account.getPassword())){
			ret.put("type", "error");
			ret.put("msg", "密码错误！");
			return ret;
		}
		request.getSession().setAttribute("account", findByName);
		ret.put("type", "success");
		ret.put("msg", "登录成功!");
		return ret;
	}
	
	
	@RequestMapping(value="/update_info",method=RequestMethod.POST)
	@ResponseBody
	public Map<String,String> updateInfo(Account account,HttpServletRequest request){
		Map<String, String> ret = new HashMap<String, String>();
		if(account == null){
			ret.put("type", "error");
			ret.put("msg", "请填写正确的信息！");
			return ret;
		}
		if(StringUtils.isEmpty(account.getAddress())){
			ret.put("type", "error");
			ret.put("msg", "地址不能为空！");
			return ret;
		}
		if(StringUtils.isEmpty(account.getPhone())){
			ret.put("type", "error");
			ret.put("msg", "手机号不能为空！");
			return ret;
		}
		if(StringUtils.isEmpty(account.getRealName())){
			ret.put("type", "error");
			ret.put("msg", "真实名称不能为空！");
			return ret;
		}
		if(StringUtils.isEmpty(account.getPassword())){
			ret.put("type", "error");
			ret.put("msg", "密码不能为空！");
			return ret;
		}
		Object attribute = request.getSession().getAttribute("account");
		if(attribute == null){
			ret.put("type", "error");
			ret.put("msg", "您还未登录！");
			return ret;
		}
		Account loginedAccount = (Account)attribute;
		loginedAccount.setAddress(account.getAddress());
		loginedAccount.setPassword(account.getPassword());
		loginedAccount.setPhone(account.getPhone());
		loginedAccount.setRealName(account.getRealName());
		if(accountService.edit(loginedAccount) <= 0){
			ret.put("type", "error");
			ret.put("msg", "修改失败，请联系管理员！");
			return ret;
		}
		ret.put("type", "success");
		ret.put("msg", "修改成功!");
		return ret;
	}
	
	
	@RequestMapping(value="/cancel_order",method=RequestMethod.POST)
	@ResponseBody
	public Map<String,String> cancelOrder(Long id){

		Map<String, String> ret = new HashMap<String, String>();
		if(orderService.elter(id) <= 0){
			ret.put("type", "error");
			ret.put("msg", "修改失败，请联系管理员！");
			return ret;
		}
		ret.put("type", "success");
		ret.put("msg", "取消成功！");
		return ret;
	}
	
	
	/**
	 * 判断用户名是否存在
	 * @param name
	 * @param id
	 * @return
	 */
	private boolean isExist(String name,Long id){
		Account account = accountService.findByName(name);
		if(account == null)return false;
		if(account.getId().longValue() == id.longValue())return false;
		return true;
	}
}
