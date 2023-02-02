package dokodemo

import "github.com/p4gefau1t/trojan-go/config"

type Config struct {
	LocalHost  string `json:"local_addr" yaml:"local-addr"`
	LocalPort  int    `json:"local_port" yaml:"local-port"`
	TargetHost string `json:"target_addr" yaml:"target-addr"`
	TargetPort int    `json:"target_port" yaml:"target-port"`
	UDPTimeout int    `json:"udp_timeout" yaml:"udp-timeout"`
}

func init() {
	config.RegisterConfigCreator(Name, func() interface{} {
		return &Config{
			UDPTimeout: 60,
		}
	})
}
